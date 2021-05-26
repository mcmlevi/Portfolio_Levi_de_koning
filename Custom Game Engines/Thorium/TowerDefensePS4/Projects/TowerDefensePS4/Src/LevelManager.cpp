#include "pch.h"
#include "LevelManager.h"
#include <Core/WorldManager.h>
#include <Core/ResourceManager.h>
#include <Components/TransformComponent.h>
#include <InteractAbleComponent.h>
#include <Components/ModelComponent.h>
#include "JsonParser.h"
#include "PathFindingComponent.h"
#include "Events/InputHandler.h"
#include "Events/EventMessenger.h"
#include "EnemyComponent.h"
LevelManager::LevelManager() : m_LevelIsLoading(false)
{
	Th::EventMessenger::GetInstance().ConnectListener<entt::entity&>("ActiveCameraSet", &LevelManager::SetCamera, this);
	m_EnemyBeginPosition = glm::vec2(0, 0);
	m_EnemyTargetPosition = glm::vec2(0, 0);
	Th::WorldManager& worldManager = Th::WorldManager::GetSingleton();
	m_AllTiles2D = TileMap{ (unsigned int)m_MapDimensions.x, (unsigned int)m_MapDimensions.y };

	Th::TransformComponent transform(glm::vec3(0, 0, 0));
	m_TileMapEntity = worldManager.CreateEntity(std::string("LevelManager"), transform);
	worldManager.GetEntityRegister().emplace<PathFindingComponent>(m_TileMapEntity, this);
	Th::InputHandler::getInstance().GetMouseButtonPressedDelegate().Connect(&LevelManager::OnMouseDown, this);
}

LevelManager::~LevelManager() {

}

void LevelManager::LoadLevel(std::string level) {
	m_EnemyBeginPositionSet = false;
	m_EnemyTargetPositionSet = false;

	m_LevelIsLoading = true;
	Th::ResourceManager& resourceManager = Th::ResourceManager::GetInstance();
	Th::WorldManager& worldManager = Th::WorldManager::GetSingleton();

	//TODO add ifdef for different json parser on ps4
	JsonParser json_parser;
	json_parser.Parse(level);
	if (json_parser.m_ActiveParsedFile != nullptr) {
		std::cout << "reading file worked" << std::endl;
		SetTargetJsonFile(*json_parser.m_ActiveParsedFile);
	}
	else {
		std::cout << "loading level not completed due to inccorect file" << std::endl;
		return;
	}
	PlaceMainTiles();
	PlaceConnectiveTiles();
	SetAllNeighbouringTiles();
	SetconnectiveTileModels();
	SetAllTileInteractAbles();
	PlaceEnemyBeginEndPoint();
	//Get all path elemtns from ogmo function TODO
	if (m_EnemyBeginPositionSet && m_EnemyTargetPositionSet) {
		CalculatePreferredPath();
		CalculateFullPath();
	}



	for (unsigned int x = 0; x < 59; x++) {
		for (unsigned int y = 0; y < 59; y++) {
			if (m_AllTiles2D(x, y).m_Initialized) {
				if (m_AllTiles2D(x, y).m_Passable) {
					auto& interactAbleComponent = Th::WorldManager::GetSingleton().GetEntityRegister().get<InteractAbleComponent>(m_AllTiles2D(x, y).m_TileInteractableEntity);  //THIS
					if (interactAbleComponent.m_CurrentInteractableType != interactAbleComponent.Checkpoint && interactAbleComponent.m_CurrentInteractableType != interactAbleComponent.BeginEndPoint) {
						auto& transform = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(m_AllTiles2D(x, y).m_TileEntity);
						//transform.SetScale(glm::vec3(0.5, 0.5, 0.5));
					}
				}
			}
		}
	}

}

void LevelManager::PlaceMainTiles() {
	int TotalHeightLayers = m_ActiveParsedLevel["TileLayers"].size();
	for (int z = 0; z < TotalHeightLayers; z++) {
		auto data = m_ActiveParsedLevel["TileLayers"][z]["data2D"].get<std::vector<std::vector<json::number_integer_t>>>();
		auto GridWidth = m_ActiveParsedLevel["TileLayers"][z]["gridCellsX"].get<json::number_integer_t>();
		auto GridHeight = m_ActiveParsedLevel["TileLayers"][z]["gridCellsY"].get<json::number_integer_t>();
		for (unsigned int y = 0; y < GridHeight; y++) {
			for (unsigned int x = 0; x < GridWidth; x++) {
				int value = data[x][y];
				if (data[x][y] != -1) {
					m_AllTiles2D(x * 2, y * 2) = Tile(glm::vec3(x * 2, z * 0.5f, y * 2), m_TileMapEntity);
					m_AllTiles2D(x * 2, y * 2).m_IsMainTile = true;
					m_AllTiles2D(x * 2, y * 2).SetTileType(m_AllTiles2D(x * 2, y * 2).FlatTile);
				}
			}
		}
	}
}

void LevelManager::PlaceConnectiveTiles() {
	for (int x = 0; x < m_MapDimensions.x; x++) {
		for (int y = 0; y < m_MapDimensions.y; y++) {
			if (m_AllTiles2D(x, y).m_Initialized == true) {
				if (m_AllTiles2D(x, y).m_IsMainTile == true) {
					int neighbors = 0;
					for (int nx = x - 2; nx <= x + 2; nx++) {
						for (int ny = y - 2; ny <= y + 2; ny++) {
							if (nx != x || ny != y) {
								if (InField(nx, 0, 58) && InField(ny, 0, 58)) { // Check if value we are going to check is inside the array values.
									if (m_AllTiles2D(nx, ny).m_Initialized == true) {
										if (m_AllTiles2D(nx, ny).m_IsMainTile == true) {
											neighbors++;
											//now comparing the difference direction
											glm::vec2 difference = glm::vec2(nx - x, ny - y);
											if (difference.x > 0) difference.x = 1;
											if (difference.x < 0) difference.x = -1;
											if (difference.y > 0) difference.y = 1;
											if (difference.y < 0) difference.y = -1;

											float height = -10;

											if (m_AllTiles2D(nx, ny).m_TilePosition.y > m_AllTiles2D(x, y).m_TilePosition.y) {
												height = m_AllTiles2D(x, y).m_TilePosition.y;
											}
											else {
												height = m_AllTiles2D(nx, ny).m_TilePosition.y;
											}


											if (m_AllTiles2D(x + difference.x, y + difference.y).m_Initialized == false) {
												m_AllTiles2D(x + difference.x, y + difference.y) = Tile(glm::vec3(x + difference.x, height, y + difference.y), m_TileMapEntity);
												m_AllTiles2D(x + difference.x, y + difference.y).m_IsMainTile = false;
												m_AllTiles2D(x + difference.x, y + difference.y).m_TilePosition.y;
											}
											else {
												if (m_AllTiles2D(x + difference.x, y + difference.y).m_TilePosition.y > m_AllTiles2D(x, y).m_TilePosition.y) {
													height = m_AllTiles2D(x, y).m_TilePosition.y;
													m_AllTiles2D(x + difference.x, y + difference.y).m_TilePosition = glm::vec3(x + difference.x, height, y + difference.y);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void LevelManager::SetAllNeighbouringTiles() {
	for (unsigned int x = 0; x < m_MapDimensions.x; x++) {
		for (unsigned int y = 0; y < m_MapDimensions.y; y++) {
			GetAllNeighboringTiles(&m_AllTiles2D(x, y));
		}
	}
}

void LevelManager::SetconnectiveTileModels() {
	for (unsigned int x = 0; x < m_MapDimensions.x; x++) {
		for (unsigned int y = 0; y < m_MapDimensions.y; y++) {
			if (m_AllTiles2D(x, y).m_Initialized) {
				if (m_AllTiles2D(x, y).m_IsMainTile == false) {
					bool ModelSet = false;
					bool lowerHeights = true;
					for (unsigned int i = 0; i < 8; i++) {
						if (m_AllTiles2D(x, y).m_SideTiles[i] != nullptr) {
							if (m_AllTiles2D(x, y).m_SideTiles[i]->m_TilePosition.y > m_AllTiles2D(x, y).m_TilePosition.y) {
								lowerHeights = false;
							}
						}
					}
					if (lowerHeights == true) {
						m_AllTiles2D(x, y).SetTileEntity(m_AllTileModels[MainTile], m_TileMapEntity);
						m_AllTiles2D(x, y).SetTileType(m_AllTiles2D(x, y).FlatTile);
					}
					//Continue to check for tile options
					else if (lowerHeights == false) {
						//First we check for the left and the right for diagonal difference

						bool HorizontalTilesNoDifference = true;

						if (m_AllTiles2D(x, y).m_SideTiles[Left] != nullptr && m_AllTiles2D(x, y).m_SideTiles[Right] != nullptr) {
							if (m_AllTiles2D(x, y).m_SideTiles[Left]->m_TilePosition.y != m_AllTiles2D(x, y).m_TilePosition.y || m_AllTiles2D(x, y).m_SideTiles[Right]->m_TilePosition.y != m_AllTiles2D(x, y).m_TilePosition.y)
							{
								HorizontalTilesNoDifference = false;
							}
						}

						if (m_AllTiles2D(x, y).m_SideTiles[Up] != nullptr && m_AllTiles2D(x, y).m_SideTiles[Down] != nullptr) {
							if (HorizontalTilesNoDifference) {
								if (m_AllTiles2D(x, y).m_SideTiles[Up]->m_TilePosition.y != m_AllTiles2D(x, y).m_SideTiles[Down]->m_TilePosition.y) {

									if (m_AllTiles2D(x, y).m_SideTiles[Up]->m_TilePosition.y > m_AllTiles2D(x, y).m_SideTiles[Down]->m_TilePosition.y) {
										m_AllTiles2D(x, y).SetTileEntity(m_AllTileModels[DiagonalTileUp], m_TileMapEntity);

										auto& transform = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(m_AllTiles2D(x, y).m_TileEntity);
										transform.SetRotation(glm::vec3(0, glm::radians(90.0f), 0));
									}
									else if (m_AllTiles2D(x, y).m_SideTiles[Up]->m_TilePosition.y < m_AllTiles2D(x, y).m_SideTiles[Down]->m_TilePosition.y) {
										m_AllTiles2D(x, y).SetTileEntity(m_AllTileModels[DiagonalTileUp], m_TileMapEntity);
										auto& transform = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(m_AllTiles2D(x, y).m_TileEntity);
										transform.SetRotation(glm::vec3(0, glm::radians(-90.0f), 0));
									}
									m_AllTiles2D(x, y).SetTileType(m_AllTiles2D(x, y).DiagonalTile);
									ModelSet = true;
								}

							}
						}
						if (!HorizontalTilesNoDifference) {
							bool VerticalTilesNoDifference = true;
							if (m_AllTiles2D(x, y).m_SideTiles[Up] != nullptr && m_AllTiles2D(x, y).m_SideTiles[Down] != nullptr) {
								if (m_AllTiles2D(x, y).m_SideTiles[Up]->m_TilePosition.y != m_AllTiles2D(x, y).m_TilePosition.y || m_AllTiles2D(x, y).m_SideTiles[Down]->m_TilePosition.y != m_AllTiles2D(x, y).m_TilePosition.y)
								{
									VerticalTilesNoDifference = false;
								}
							}
							if (m_AllTiles2D(x, y).m_SideTiles[Left] != nullptr && m_AllTiles2D(x, y).m_SideTiles[Right] != nullptr) {
								if (VerticalTilesNoDifference) {
									if (m_AllTiles2D(x, y).m_SideTiles[Left]->m_TilePosition.y != m_AllTiles2D(x, y).m_SideTiles[Right]->m_TilePosition.y) {
										if (m_AllTiles2D(x, y).m_SideTiles[Left]->m_TilePosition.y > m_AllTiles2D(x, y).m_SideTiles[Right]->m_TilePosition.y) {
											m_AllTiles2D(x, y).SetTileEntity(m_AllTileModels[DiagonalTileUp], m_TileMapEntity);
										}
										else if (m_AllTiles2D(x, y).m_SideTiles[Left]->m_TilePosition.y < m_AllTiles2D(x, y).m_SideTiles[Right]->m_TilePosition.y) {
											m_AllTiles2D(x, y).SetTileEntity(m_AllTileModels[DiagonalTileUp], m_TileMapEntity);
											auto& transform = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(m_AllTiles2D(x, y).m_TileEntity);
											transform.SetRotation(glm::vec3(0, glm::radians(180.0f), 0));
										}
										m_AllTiles2D(x, y).SetTileType(m_AllTiles2D(x, y).DiagonalTile);
										ModelSet = true;
									}
								}
							}
						}
						//Check the up and down, if that's a no check the left and right 
					}
					if (ModelSet == false) {
						Tile* SMainTiles[4] = { nullptr };
						SMainTiles[0] = m_AllTiles2D(x, y).m_SideTiles[LeftUp];
						SMainTiles[1] = m_AllTiles2D(x, y).m_SideTiles[RightUp];
						SMainTiles[2] = m_AllTiles2D(x, y).m_SideTiles[RightDown];
						SMainTiles[3] = m_AllTiles2D(x, y).m_SideTiles[LeftDown];

						int higherNeighbors = 0;
						unsigned int totalIndex = 0;
						// Check if 1 main tile is higher
						for (unsigned int i = 0; i < 4; i++) {
							if (SMainTiles[i] != nullptr) {
								if (SMainTiles[i]->m_TilePosition.y > m_AllTiles2D(x, y).m_TilePosition.y) {
									higherNeighbors++;
									totalIndex += i;
								}
							}
						}

						if (higherNeighbors == 1) {
							m_AllTiles2D(x, y).SetTileEntity(m_AllTileModels[CornerTileUp], m_TileMapEntity);
							m_AllTiles2D(x, y).SetTileType(m_AllTiles2D(x, y).CornerTileDownWards);
							auto& transform = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(m_AllTiles2D(x, y).m_TileEntity);
							transform.SetRotation(glm::vec3(0, glm::radians(90.0f * totalIndex), 0));
						}

						else if (higherNeighbors == 3) {
							m_AllTiles2D(x, y).SetTileEntity(m_AllTileModels[CornerTileDown], m_TileMapEntity);
							m_AllTiles2D(x, y).SetTileType(m_AllTiles2D(x, y).CornerTileUpwards);
							auto& transform = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(m_AllTiles2D(x, y).m_TileEntity);
							switch (totalIndex) {
							case 4: { //LeftUP corner
								transform.SetRotation(glm::vec3(0, glm::radians(0.0f), 0));
							}
								  break;
							case 3: { //RightUp corner
								transform.SetRotation(glm::vec3(0, glm::radians(90.0f), 0));
							}
								  break;
							case 6: { //RightDown corner
								transform.SetRotation(glm::vec3(0, glm::radians(180.0f), 0));
							}
								  break;
							case 5: { //LeftDown corner
								transform.SetRotation(glm::vec3(0, glm::radians(270.0f), 0));
							}
								  break;

							}
						}
					}

				}
				//Placing flat tiles models on maintiles.
				else if (m_AllTiles2D(x, y).m_IsMainTile == true) {
					m_AllTiles2D(x, y).SetTileEntity(m_AllTileModels[MainTile], m_TileMapEntity);
					m_AllTiles2D(x, y).SetTileType(m_AllTiles2D(x, y).FlatTile);
				}

			}
		}
	}
}

void LevelManager::SetAllTileInteractAbles() {
	const auto data = m_ActiveParsedLevel["InteractAbles"][0]["data2D"].get<std::vector<std::vector<json::number_integer_t>>>();
	auto GridWidth = m_ActiveParsedLevel["TileLayers"][0]["gridCellsX"].get<json::number_integer_t>();
	auto GridHeight = m_ActiveParsedLevel["TileLayers"][0]["gridCellsY"].get<json::number_integer_t>();
	for (unsigned int y = 0; y < GridHeight; y++) {
		for (unsigned int x = 0; x < GridWidth; x++) {
			//First setting the main tile interactables
			int value = data[x][y];
			if (value != -1) {
				auto& interactAbleComponent = Th::WorldManager::GetSingleton().GetEntityRegister().get<InteractAbleComponent>(m_AllTiles2D(x * 2, y * 2).m_TileInteractableEntity);  //THIS
				ModelComponent* finalModel = nullptr;
				int tileType = m_AllTiles2D(x * 2, y * 2).currentTileType;
				bool rotateAble = false;

				if (m_AllTiles2D(x * 2, y * 2).m_Initialized) {
					if (m_AllTiles2D(x * 2, y * 2).m_IsMainTile) {

						if (value == 0) {
							//Tree
							int randomTree = rand() % 3;
							finalModel = GetTileRelatedModel(randomTree, 0);
							interactAbleComponent.SetInteractableType(interactAbleComponent.Tree);
							m_AllTiles2D(x * 2, y * 2).m_Passable = false;
						}
						else if (value == 1) {
							//Obstacle
							int randomObstacle = rand() % 2 + 3;
							finalModel = GetTileRelatedModel(randomObstacle, 0);
							interactAbleComponent.SetInteractableType(interactAbleComponent.Obstacle);
							m_AllTiles2D(x * 2, y * 2).m_Passable = false;
						}
						else if (value == 2) {
							//grass
							finalModel = GetTileRelatedModel(5, 0);
							interactAbleComponent.SetInteractableType(interactAbleComponent.Grass);
							m_AllTiles2D(x * 2, y * 2).m_Passable = true;
						}

						else {
							finalModel = GetTileRelatedModel(5 + value - 2, 0);
							interactAbleComponent.SetInteractableType(interactAbleComponent.Rock);
							m_AllTiles2D(x * 2, y * 2).m_Passable = false;
						}

						int randomRotation = rand() % 4;
						auto& transform = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(m_AllTiles2D(x * 2, y * 2).m_TileEntity);
						auto& InterActAbletransform = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(m_AllTiles2D(x * 2, y * 2).m_TileInteractableEntity);
						InterActAbletransform.SetRotation(glm::vec3(0, glm::radians(float(randomRotation * 90)), 0));
						InterActAbletransform.SetPosition(transform.GetPosition());
						m_AllTiles2D(x * 2, y * 2).SetInteractableEntity(finalModel);

					}
				}
			}
			else {
				//auto& interactAbleComponent = Th::WorldManager::GetSingleton().GetEntityRegister().get<InteractAbleComponent>(m_AllTiles2D(x, y).m_TileInteractableEntity);  //THIS
				//interactAbleComponent.m_HasI = false;
			}
		}
	}
	ModelComponent* finalModel = nullptr;
	bool NhasI = false;
	for (int j = 0; j < 59; j++) {
		for (int k = 0; k < 59; k++) {
			if (m_AllTiles2D(j, k).m_Initialized) {
				if (m_AllTiles2D(j, k).m_IsMainTile == false) {
					//finalModel = GetTileRelatedModel(5, m_AllTiles2D(j, k).currentTileType);
					int grassNeighbors = 0;
					int treeNeighbors = 0;
					for (unsigned int i = 0; i < 8; i++) {
						if (m_AllTiles2D(j, k).m_SideTiles[i] != nullptr) {
							if (m_AllTiles2D(j, k).m_SideTiles[i]->m_IsMainTile) {
								auto& SideTileInteractable = Th::WorldManager::GetSingleton().GetEntityRegister().get<InteractAbleComponent>(m_AllTiles2D(j, k).m_SideTiles[i]->m_TileInteractableEntity);  //THIS
								if (m_AllTiles2D(j, k).m_SideTiles[i]->m_HasInteractablePlaced) {
									NhasI = true;
									if (SideTileInteractable.m_CurrentInteractableType == SideTileInteractable.Tree) {
										treeNeighbors++;
									}
									else if (SideTileInteractable.m_CurrentInteractableType == SideTileInteractable.Grass) {
										grassNeighbors++;
									}
								}
								else {
									NhasI = false;
								}
							}
						}
					}
					if (NhasI) {
						if (grassNeighbors == treeNeighbors) {
							//Place grass
							finalModel = GetTileRelatedModel(5, m_AllTiles2D(j, k).currentTileType);
							m_AllTiles2D(j, k).m_Passable = true;
						}
						else if (grassNeighbors > treeNeighbors) {
							//Place grass
							finalModel = GetTileRelatedModel(5, m_AllTiles2D(j, k).currentTileType);
							m_AllTiles2D(j, k).m_Passable = true;
						}
						else {
							//place tree
							int randomTree = rand() % 3;
							finalModel = GetTileRelatedModel(randomTree, m_AllTiles2D(j, k).currentTileType);
							m_AllTiles2D(j, k).m_Passable = false;
						}


						if (finalModel != nullptr) {
							auto& transform = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(m_AllTiles2D(j, k).m_TileEntity);
							auto& InterActAbletransform = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(m_AllTiles2D(j, k).m_TileInteractableEntity);
							InterActAbletransform.SetRotation(transform.GetRotation());
							InterActAbletransform.SetPosition(transform.GetPosition());
							m_AllTiles2D(j, k).SetInteractableEntity(finalModel);
						}

					}
				}

			}
		}
	}
}

void LevelManager::PlaceEnemyBeginEndPoint() {
	for (int i = 0; i < m_ActiveParsedLevel["EntitieData"][0]["entities"].size(); i++) {
		ModelComponent* finalModel = nullptr;
		glm::vec2 OgmoEntityPosition;
		//Getting data
		OgmoEntityPosition.y = ((m_ActiveParsedLevel["EntitieData"][0]["entities"][i]["x"].get<json::number_float_t>() / 64) * 2);
		OgmoEntityPosition.x = ((m_ActiveParsedLevel["EntitieData"][0]["entities"][i]["y"].get<json::number_float_t>() / 64) * 2);
		std::string OgmoEntityName = m_ActiveParsedLevel["EntitieData"][0]["entities"][i]["name"].get<json::string_t>();
		auto& interactAbleComponent = Th::WorldManager::GetSingleton().GetEntityRegister().get<InteractAbleComponent>(m_AllTiles2D(OgmoEntityPosition.x, OgmoEntityPosition.y).m_TileInteractableEntity);  //THIS
		//Checking if the tile already contains a interactable
		if (m_AllTiles2D(OgmoEntityPosition.x, OgmoEntityPosition.y).m_HasInteractablePlaced == false) {
			//Checking name of the data we are reading
			if (OgmoEntityName == "EnemyBeginPosition") {
				finalModel = m_AllTowerModels[0];
				m_EnemyBeginPosition = OgmoEntityPosition;
				m_EnemyBeginTile = &m_AllTiles2D(OgmoEntityPosition.x, OgmoEntityPosition.y);
				interactAbleComponent.SetInteractableType(interactAbleComponent.BeginEndPoint);
				m_EnemyBeginPositionSet = true;
			}
			else if (OgmoEntityName == "EnemyTarget") {
				finalModel = m_AllTowerModels[1];
				m_EnemyTargetPosition = OgmoEntityPosition;
				m_EnemyTargetTile = &m_AllTiles2D(OgmoEntityPosition.x, OgmoEntityPosition.y);
				interactAbleComponent.SetInteractableType(interactAbleComponent.BeginEndPoint);
				m_EnemyTargetPositionSet = true;
			}
			//Placing final model
			if (finalModel != nullptr) {
				m_AllTiles2D(OgmoEntityPosition.x, OgmoEntityPosition.y).SetInteractableEntity(finalModel);
				m_AllTiles2D(OgmoEntityPosition.x, OgmoEntityPosition.y).m_Passable = true;
			}

		}
		else {
			std::cout << "already has interactable" << std::endl;
		}

		//Setting all the surrounding tiles that do not hold a interactable, to hold a grass interactable
		SetEmptySideTilesInteractAble(&m_AllTiles2D(OgmoEntityPosition.x, OgmoEntityPosition.y));
	}

}

void LevelManager::SetCamera(entt::entity& newCamera) {
	m_Camera = newCamera;
}

void LevelManager::CreateLevelArrow() {
	Th::WorldManager& worldManager = Th::WorldManager::GetSingleton();
	Th::TransformComponent Arrowtransform(glm::vec3(0, 10000, 0));
	m_LevelArrow = worldManager.CreateEntity(std::string("LevelArrow"), Arrowtransform);
	worldManager.GetEntityRegister().emplace<ModelComponent>(m_LevelArrow, *m_ArrowModel);
}

void LevelManager::CalculateFullPath() {
	//Use pathfinding to calculate the paths inbetween the different target tiles
	m_GeneratedPath.clear();
	auto& pathfindingComponent = Th::WorldManager::GetSingleton().GetEntityRegister().get<PathFindingComponent>(m_TileMapEntity);
	pathfindingComponent.FindPath(m_EnemyBeginPosition, m_EnemyTargetPosition);

	m_GeneratedPath = pathfindingComponent.m_LastRetracedPath;
}

void LevelManager::CalculatePreferredPath() {
	m_PreferredPath.clear();
	//creating and setting the size of the m_Alltargets vector, which will hold a reference to the target tiles inbetween the begin and end position, defined in ogmo.
	int TargetAmount = m_ActiveParsedLevel["PathPositions"][0]["TileElements"].size() + 1; //+1 because we are adding the already known last tile as well.
	std::vector<Tile*> m_AllTargets;
	m_AllTargets.resize(TargetAmount);

	//Setting all the target elements, using the ogmo data
	for (unsigned int k = 0; k < m_AllTargets.size(); k++) {
		for (int i = 0; i < m_ActiveParsedLevel["PathPositions"][0]["TileElements"].size(); i++) {
			int PathStep = (m_ActiveParsedLevel["PathPositions"][0]["TileElements"][i]["values"]["PathStep"].get<json::number_integer_t>());
			if (PathStep == k) {
				glm::vec2 position;
				position.x = ((m_ActiveParsedLevel["PathPositions"][0]["TileElements"][i]["y"].get<json::number_integer_t>() / 64) * 2);
				position.y = ((m_ActiveParsedLevel["PathPositions"][0]["TileElements"][i]["x"].get<json::number_integer_t>() / 64) * 2);
				m_AllTargets[k] = &m_AllTiles2D(position.x, position.y);

				//Setting the interactable variables
				auto& interactAbleComponent = Th::WorldManager::GetSingleton().GetEntityRegister().get<InteractAbleComponent>(m_AllTiles2D(position.x, position.y).m_TileInteractableEntity);  //THIS
				m_AllTiles2D(position.x, position.y).SetInteractableEntity(m_CheckPointModel);
				interactAbleComponent.SetInteractableType(interactAbleComponent.Checkpoint);

				//Adding grass to the surrounding tiles
				SetEmptySideTilesInteractAble(&m_AllTiles2D(position.x, position.y));
			}
		}
	}
	//setting the last element, which will be the already known final target tile.
	m_AllTargets[m_AllTargets.size() - 1] = &m_AllTiles2D(m_EnemyTargetPosition.x, m_EnemyTargetPosition.y);
	for (unsigned int y = 0; y < m_AllTargets.size(); y++) {
		auto& transform = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(m_AllTargets[y]->m_TileEntity);
		//transform.SetScale(glm::vec3(0.5, 0.5, 0.5));
	}

	//Use pathfinding to calculate the paths inbetween the different target tiles
	auto& pathfindingComponent = Th::WorldManager::GetSingleton().GetEntityRegister().get<PathFindingComponent>(m_TileMapEntity);

	//Finding the path for each segment of the whole path using the checkpoints from the ogmo data
	Tile* BeginCheckTile = &m_AllTiles2D(m_EnemyBeginPosition.x, m_EnemyBeginPosition.y);
	Tile* TargetCheckTile = m_AllTargets[0];
	for (unsigned int i = 0; i < m_AllTargets.size(); i++) {
		pathfindingComponent.FindPath(glm::vec2(BeginCheckTile->m_TilePosition.x, BeginCheckTile->m_TilePosition.z), glm::vec2(TargetCheckTile->m_TilePosition.x, TargetCheckTile->m_TilePosition.z));
		m_PreferredPath.insert(m_PreferredPath.end(), pathfindingComponent.m_LastRetracedPath.begin(), pathfindingComponent.m_LastRetracedPath.end());
		if (m_AllTargets[i] != &m_AllTiles2D(m_EnemyTargetPosition.x, m_EnemyTargetPosition.y)) {
			BeginCheckTile = m_AllTargets[i];
			TargetCheckTile = m_AllTargets[i + 1];
		}
	}

	for (unsigned int i = 0; i < m_PreferredPath.size(); i++) {
		m_PreferredPath[i]->m_PreferredTile = true;
		m_PreferredPath[i]->movementPenalty = 0;
	}
	SetDistancePenalty();
}

void LevelManager::SetDistancePenalty() {
	for (int j = 0; j < 59; j++) {
		for (int k = 0; k < 59; k++) {
			if (m_AllTiles2D(j, k).m_Initialized) {
				if (m_AllTiles2D(j, k).m_Passable == true) {
					if (m_AllTiles2D(j, k).m_PreferredTile == false) {
						int closest = 100;
						for (unsigned int i = 0; i < m_PreferredPath.size(); i++) {
							int distance = glm::distance(m_PreferredPath[i]->m_TilePosition, m_AllTiles2D(j, k).m_TilePosition);
							if (distance < closest) {
								closest = distance;
							}
						}
						m_AllTiles2D(j, k).movementPenalty = closest * 25; //TODO check this multiplyer value
					}
				}
			}
		}
	}
}


void LevelManager::SetTileModelShader(Th::RenderCore* renderCore) {
	Th::ResourceManager& resourceManager = Th::ResourceManager::GetInstance();
	Th::WorldManager& worldManager = Th::WorldManager::GetSingleton();

	//Tiles
	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/Tiles/Tile.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/Tiles/Tile.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllTileModels[0] = resourceManager.GetModel("[models]/Tiles/Tile.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/Tiles/DiagonalConnectionTileUpwardsStraight.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/Tiles/DiagonalConnectionTileUpwardsStraight.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllTileModels[1] = resourceManager.GetModel("[models]/Tiles/DiagonalConnectionTileUpwardsStraight.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/Tiles/CornerConnectionUpwards.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/Tiles/CornerConnectionUpwards.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllTileModels[2] = resourceManager.GetModel("[models]/Tiles/CornerConnectionUpwards.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/Tiles/CornerConnectionDownWards.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/Tiles/CornerConnectionDownWards.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllTileModels[3] = resourceManager.GetModel("[models]/Tiles/CornerConnectionDownWards.obj");

	//Single tree
	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/TreeSingle/TreeSingleFlatTile.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/TreeSingle/TreeSingleFlatTile.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[0][0] = resourceManager.GetModel("[models]/TreeSingle/TreeSingleFlatTile.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/TreeSingle/TreeSingleDiagonalTile.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/TreeSingle/TreeSingleDiagonalTile.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[0][1] = resourceManager.GetModel("[models]/TreeSingle/TreeSingleDiagonalTile.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/TreeSingle/TreeSingleCornerUpwards.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/TreeSingle/TreeSingleCornerUpwards.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[0][2] = resourceManager.GetModel("[models]/TreeSingle/TreeSingleCornerUpwards.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/TreeSingle/TreeSingleCornerDownWards.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/TreeSingle/TreeSingleCornerDownWards.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[0][3] = resourceManager.GetModel("[models]/TreeSingle/TreeSingleCornerDownWards.obj");

	//Double tree
	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/TreeDouble/TreeDoubleFlatTile.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/TreeDouble/TreeDoubleFlatTile.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[1][0] = resourceManager.GetModel("[models]/TreeDouble/TreeDoubleFlatTile.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/TreeDouble/TreeDoubleDiagonalTile.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/TreeDouble/TreeDoubleDiagonalTile.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[1][1] = resourceManager.GetModel("[models]/TreeDouble/TreeDoubleDiagonalTile.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/TreeDouble/TreeDoubleCornerUpwards.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/TreeDouble/TreeDoubleCornerUpwards.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[1][2] = resourceManager.GetModel("[models]/TreeDouble/TreeDoubleCornerUpwards.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/TreeDouble/TreeDoubleCornerDownWards.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/TreeDouble/TreeDoubleCornerDownWards.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[1][3] = resourceManager.GetModel("[models]/TreeDouble/TreeDoubleCornerDownWards.obj");

	//Quad tree
	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/TreeQuad/TreeQuadFlatTile.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/TreeQuad/TreeQuadFlatTile.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[2][0] = resourceManager.GetModel("[models]/TreeQuad/TreeQuadFlatTile.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/TreeQuad/TreeQuadDiagonalTile.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/TreeQuad/TreeQuadDiagonalTile.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[2][1] = resourceManager.GetModel("[models]/TreeQuad/TreeQuadDiagonalTile.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/TreeQuad/TreeQuadCornerUpWards.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/TreeQuad/TreeQuadCornerUpWards.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[2][2] = resourceManager.GetModel("[models]/TreeQuad/TreeQuadCornerUpWards.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/TreeQuad/TreeQuadCornerDownWards.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/TreeQuad/TreeQuadCornerDownWards.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[2][3] = resourceManager.GetModel("[models]/TreeQuad/TreeQuadCornerDownWards.obj");

	//small Obstacles
	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/Obstacle/ObstacleSmallTileFlat.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/Obstacle/ObstacleSmallTileFlat.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[3][0] = resourceManager.GetModel("[models]/Obstacle/ObstacleSmallTileFlat.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/Obstacle/ObstacleSmallTileDiagonal.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/Obstacle/ObstacleSmallTileDiagonal.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[3][1] = resourceManager.GetModel("[models]/Obstacle/ObstacleSmallTileDiagonal.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/Obstacle/ObstacleSmallCornerUPWards.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/Obstacle/ObstacleSmallCornerUPWards.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[3][2] = resourceManager.GetModel("[models]/Obstacle/ObstacleSmallCornerUPWards.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/Obstacle/ObstacleSmallCornerDownWards.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/Obstacle/ObstacleSmallCornerDownWards.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[3][3] = resourceManager.GetModel("[models]/Obstacle/ObstacleSmallCornerDownWards.obj");

	//High Obstacles
	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/ObstacleBig/ObstacleHighTileFlat.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/ObstacleBig/ObstacleHighTileFlat.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[4][0] = resourceManager.GetModel("[models]/ObstacleBig/ObstacleHighTileFlat.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/ObstacleBig/ObstacleHighTileDiagonal.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/ObstacleBig/ObstacleHighTileDiagonal.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[4][1] = resourceManager.GetModel("[models]/ObstacleBig/ObstacleHighTileDiagonal.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/ObstacleBig/ObstacleHighCornerUPWards.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/ObstacleBig/ObstacleHighCornerUPWards.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[4][2] = resourceManager.GetModel("[models]/ObstacleBig/ObstacleHighCornerUPWards.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/ObstacleBig/ObstacleHighCornerDownWards.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/ObstacleBig/ObstacleHighCornerDownWards.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[4][3] = resourceManager.GetModel("[models]/ObstacleBig/ObstacleHighCornerDownWards.obj");

	//Grass
	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/GrassTiles/GrassTileFlat.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/GrassTiles/GrassTileFlat.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[5][0] = resourceManager.GetModel("[models]/GrassTiles/GrassTileFlat.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/GrassTiles/GrassTileDiagonal.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/GrassTiles/GrassTileDiagonal.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[5][1] = resourceManager.GetModel("[models]/GrassTiles/GrassTileDiagonal.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/GrassTiles/GrassCornerUpWards.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/GrassTiles/GrassCornerUpWards.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[5][2] = resourceManager.GetModel("[models]/GrassTiles/GrassCornerUpWards.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/GrassTiles/GrassCronerDownWards.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/GrassTiles/GrassCronerDownWards.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[5][3] = resourceManager.GetModel("[models]/GrassTiles/GrassCronerDownWards.obj");

	//Rocks

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/Rocks/BigHill.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/Rocks/BigHill.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[6][0] = resourceManager.GetModel("[models]/Rocks/BigHill.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/Rocks/BigDirtHill.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/Rocks/BigDirtHill.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[7][0] = resourceManager.GetModel("[models]/Rocks/BigDirtHill.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/Rocks/DirtHill.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/Rocks/DirtHill.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[8][0] = resourceManager.GetModel("[models]/Rocks/DirtHill.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/Rocks/Rock.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/Rocks/Rock.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[9][0] = resourceManager.GetModel("[models]/Rocks/Rock.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/Rocks/CrystalSingle.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/Rocks/CrystalSingle.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[10][0] = resourceManager.GetModel("[models]/Rocks/CrystalSingle.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/Rocks/CrystalHill.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/Rocks/CrystalHill.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllInteractables[11][0] = resourceManager.GetModel("[models]/Rocks/CrystalHill.obj");

	//Towers
	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/EnemyTowers/EnemyBeginTower.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/EnemyTowers/EnemyBeginTower.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllTowerModels[0] = resourceManager.GetModel("[models]/EnemyTowers/EnemyBeginTower.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/EnemyTowers/EnemyEndTower.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/EnemyTowers/EnemyEndTower.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllTowerModels[1] = resourceManager.GetModel("[models]/EnemyTowers/EnemyEndTower.obj");

	//Checkpoint model 
	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/Checkpoint.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/Checkpoint.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_CheckPointModel = resourceManager.GetModel("[models]/Checkpoint.obj");

	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/Arrow.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/Arrow.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_ArrowModel = resourceManager.GetModel("[models]/Arrow.obj");

	CreateLevelArrow();
}

void LevelManager::OnMouseDown(Th::MouseButtonEventArgs mouse) {
	if (mouse.LeftButton) {
		auto& ArrowTranformComponent = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(m_LevelArrow);
		entt::entity found = Th::WorldManager::GetSingleton().RayCast<InteractAbleComponent>(glm::vec2(mouse.X, mouse.Y), m_Camera);
		if (found != entt::null) {


			auto& interactAbleComponent = Th::WorldManager::GetSingleton().GetEntityRegister().get<InteractAbleComponent>(found);  //THIS
			auto& transform = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(found);
			Tile* activeTile = &m_AllTiles2D(transform.GetPosition().x, transform.GetPosition().z);

			if (activeTile->m_Initialized) {
				if (activeTile->m_Passable) {
					if (interactAbleComponent.m_CurrentInteractableType != interactAbleComponent.Checkpoint && interactAbleComponent.m_CurrentInteractableType != interactAbleComponent.BeginEndPoint) {
						auto& transform2 = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(activeTile->m_TileEntity);
						glm::vec3 newArrowPosition = transform2.GetPosition();
						newArrowPosition.y += CalculateArrowOffset(activeTile->currentTileType);
						ArrowTranformComponent.SetPosition(newArrowPosition);
						m_SelectedTile = activeTile;
					}
				}
				else {
					ArrowTranformComponent.SetPosition(glm::vec3(0, 10000, 0));
					m_SelectedTile = nullptr;
				}
			}
		}
		else {
			ArrowTranformComponent.SetPosition(glm::vec3(0, 10000, 0));
			m_SelectedTile = nullptr;
		}
	}
}

void LevelManager::SetEmptySideTilesInteractAble(Tile* checkTile) {
	for (unsigned int i = 0; i < 8; i++) {
		if (checkTile->m_SideTiles[i] != nullptr) {
			if (checkTile->m_SideTiles[i]->m_HasInteractablePlaced == false) {
				checkTile->m_SideTiles[i]->SetInteractableEntity(GetTileRelatedModel(5, checkTile->m_SideTiles[i]->currentTileType));
				auto& InterActAbletransform = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(checkTile->m_SideTiles[i]->m_TileInteractableEntity);
				auto& transform = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(checkTile->m_SideTiles[i]->m_TileEntity);
				InterActAbletransform.SetRotation(transform.GetRotation());
				InterActAbletransform.SetPosition(transform.GetPosition());
				checkTile->m_SideTiles[i]->m_Passable = true;
			}
		}
	}
}

float LevelManager::CalculateArrowOffset(int TileType) {
	switch (TileType) {
	case 0:
		return 0.25f;
		break;
	case 1:
		return 0.5f;
		break;
	case 2:
		return 0.5f;
		break;
	case 3:
		return 0.7f;
		break;
	default:
		break;
	}
}

ModelComponent* LevelManager::GetTileRelatedModel(int interactableType, int tileType) {
	return m_AllInteractables[interactableType][tileType];
}

void LevelManager::GetAllNeighboringTiles(Tile* checkpoint) {
	std::vector<Tile*> AllSuroundingTiles;
	glm::vec2 StartPosition = glm::vec2(checkpoint->m_TilePosition.x, checkpoint->m_TilePosition.z);
	for (int i = 0; i < 8; i++) {
		if (InField(StartPosition.x + m_Sides[i].x, 0, 58) && InField(StartPosition.y + m_Sides[i].y, 0, 58)) { //Using 58 as max due to tile at row 59, 59 never being assigned.
			if (m_AllTiles2D(StartPosition.x + m_Sides[i].x, StartPosition.y + m_Sides[i].y).m_IsMainTile) {
				checkpoint->m_MainTileNeighbors++;
			}
			else {
				checkpoint->m_ConnectiveTileNeighbors++;
			}
			//Adding the found neighbor tile to a list of possible neighbors in the tile itself. will be used later on. 
			checkpoint->SetSideTile(&m_AllTiles2D(StartPosition.x + m_Sides[i].x, StartPosition.y + m_Sides[i].y), i);
		}
	}
}

bool LevelManager::InField(int checkValue, int min, int max) {
	if (checkValue < min || checkValue > max) {
		return false;
	}
	return true;
}


