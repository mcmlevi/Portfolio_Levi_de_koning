#include "pch.h"

#include "UIManager.h"
#include "CameraManager.h"
#include "Graphics/Components/TextComponent.h"

#include "GameManager.h"

UIManager::UIManager(Frac::EngineCore& engine, GameManager& gameManager) :
	m_engine(engine)
	, m_world(engine.GetWorld())
	, m_inputSystem(engine.GetInputSystem())
	, m_camEntsQuery(m_world.makeQuery<TOR::Camera>())
	, m_updateCanvasSys(m_world.makeSystem<TOR::Transform, UICanvas, Frac::TextComponent>())
	, m_lastActiveCanvasEnt(0)
	, m_cursor3DManager(std::make_unique<MouseCursorManager>(m_world))
	, m_gameManager(gameManager)
	, m_initialRun(true)
	, m_ignoreFirst(false)
	, m_activePopupEnt(0)
{
}

void UIManager::Initialise(GameState* a_gameState)
{
	
	m_gameState = a_gameState;

	// make these canvases first so main menu and pause canvases can properly initialise
	const std::string gameplayCanvasStr = CreateGameplayCanvas();
	const std::string controlsCanvasStr = CreateControlsCanvas();
	const std::string optionsCanvasStr	= CreateOptionsCanvas();
	const std::string victoryCanvasStr	= CreateVictoryCanvas();
	const std::string defeatCanvasStr	= CreateDefeatCanvas();
	
	m_menuItems = std::vector<std::pair<std::string, std::string>>({
		{gameplayCanvasStr, "Play"},
		{controlsCanvasStr, "Controls"},
		{optionsCanvasStr, "Options"}

#if defined (PLATFORM_WINDOWS)
		,{"QuitGame", "Quit"}
		});
#else
		});
#endif
	
	CreateMainMenuCanvas();
	CreatePauseCanvas();

	std::function<void(EntityHandle, TOR::Transform&, UICanvas&, Frac::TextComponent&)> updateCanvases = 
		[this](EntityHandle entity, TOR::Transform& transform, UICanvas& canvas, Frac::TextComponent& text)
	{
			UpdateCanvases(entity, transform, canvas, text);
	};
	m_updateCanvasSys = m_world.makeSystem<TOR::Transform, UICanvas, Frac::TextComponent>(updateCanvases, Frac::ePipelineStage::onUpdate, "updateCanvases");
}

void UIManager::Update()
{
	const auto popupTrans = m_world.get<TOR::Transform>(m_activePopupEnt);
	
	const glm::vec2 selectedGridpos = m_gameManager.GetSelectedGridPosition();
	if (m_gameManager.m_tileManager->IsInGrid(selectedGridpos))
	{
		const size_t curTileID = m_gameManager.m_tileManager->GetTileID(selectedGridpos);
		const TileComponent* curTileComp = m_world.get<TileComponent>(m_gameManager.m_tileManager->m_tileMap->m_tileEntities->at(curTileID));

		if(curTileComp->m_occupier != 0 && m_world.has(curTileComp->m_occupier))
		{
			m_gameManager.m_uiManager->HandlePopupEntity(curTileComp->m_occupier);
			
			m_cursor3DManager->SetSelectState();
			popupTrans->Scale = m_popupSize;
		}
		else
		{
			m_cursor3DManager->SetIdleState();
			popupTrans->Scale = glm::vec3(0.f);
		}
	}
	else
	{
		m_cursor3DManager->SetIdleState();
		popupTrans->Scale = glm::vec3(0.f);
	}
}

std::string UIManager::CreateMainMenuCanvas()
{
	std::string canvasName = "MainMenuCanvas";
	
	Frac::EntityHandle theCanvasEntity = m_world.makeEntity<TOR::Transform, UICanvas, Frac::TextComponent>(canvasName,
		{},
		{
			canvasName
		},
		{});
	
	UICanvas* theCanvas = m_world.get<UICanvas>(theCanvasEntity);
	m_canvasesMap.insert({canvasName, theCanvasEntity});
	
	{
		Frac::EntityHandle mmLogoEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_Logo", 
			{m_logoPos, m_logoSize}, 
			{}, 
			{"A" // paint icon A
			, 5.0f
			, glm::vec3(1.f)
			, Frac::TextComponent::FontFamily::Icons });
		
		m_world.addChild(
			mmLogoEnt,
			theCanvasEntity
		);

		Frac::EntityHandle mmBackdropEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_Backdrop", 
			{m_backdropPos, m_backdropSize} , 
			{} , 
			{"S" // square
			, m_backdropFontSize
			, m_backdropColor
			, Frac::TextComponent::FontFamily::Icons }
		);
		m_world.addChild(
			mmBackdropEnt,
			theCanvasEntity
		);

		{
			theCanvas->theSelectorEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_Selector", 
				{glm::vec3(-8.f, 0.f, m_buttonDepth), glm::vec3(0.5f)}, 
				{}, 
					{
						">"
						,0.5f
						, glm::vec3(0.25f, 1.f, 0.25f)
						, Frac::TextComponent::FontFamily::Icons
				});
			m_world.addChild(
				theCanvas->theSelectorEnt,
				mmBackdropEnt
			);

			int yIncrement = 0;
			for (auto it = m_menuItems.begin(); it != m_menuItems.end(); ++it)
			{
				Frac::EntityHandle mmButtonEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_" + it->second + "_Button", 
					{glm::vec3(
						0.f, 
						m_buttonVerStartOffset + ( static_cast<float>(yIncrement++) * -m_buttonVerPadding), 
						m_buttonDepth), glm::vec3(0.5f)}, 
					{
						it->first
					}, 
					{
						it->second
						,m_buttonFontSize
						, theCanvas->idleColor
					});
				m_world.addChild(
					mmButtonEnt,
					mmBackdropEnt
				);

				theCanvas->selectableElements.push_back(mmButtonEnt);
			}
		}
	}

	return canvasName;
}

std::string UIManager::CreateGameplayCanvas()
{
	std::string canvasName = "GameplayCanvas";
	
	const Frac::EntityHandle theCanvasEntity = m_world.makeEntity<TOR::Transform, UICanvas, Frac::TextComponent>(canvasName,
		{},
		{
			canvasName
		},
		{});
	
	UICanvas* theCanvas = m_world.get_mutable<UICanvas>(theCanvasEntity);
	m_canvasesMap.insert({canvasName, theCanvasEntity});
	
	// curwave, enemy incomming, enemy alive, 
	{
		/////// Counters
		const std::pair<std::string, std::string> counterElements[4]
		{
			{"ResourceCounter_0", "c"},
			{"WaveCounter_0", "E"},
			{"IncomingEnemiesCounter_0", "V"},
			{"BaseHPCounter_0", "O"}
		};
		
		for (size_t i = 0; i < 4; i++)
		{
			Frac::EntityHandle resCountIcon = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_BuildInfo", 
				{
					m_iconBasePos + (m_counterListOffset * static_cast<float>(i)),
					glm::vec3(0.5f)}, 
				{}, 
			{
				counterElements[i].second	
				,0.7f
				, m_resCountColor
				, Frac::TextComponent::FontFamily::Icons
			});
			m_world.addChild(
				resCountIcon,
				theCanvasEntity
			);

			Frac::EntityHandle resCountText = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_BuildInfo", 
				{glm::vec3(m_iconHorOffset, 0.f, -1.5f), glm::vec3(0.5f)}, 
				{}, 
			{
				""
				,0.75f
				, m_resCountColor
			});
			m_world.addChild(
				resCountText,
				resCountIcon
			);

			theCanvas->settableElements[counterElements[i].first] = resCountText;
		}
		
		// The popup
		{
			m_activePopupEnt = m_world.makeEntity<TOR::Transform, UIElement, UIPopUp, Frac::TextComponent>(canvasName + "_Popup", 
				{
					glm::vec3(0.f),
					glm::vec3(0.f),
					glm::quat(
						glm::vec3(
							glm::radians<float>(25.5f), 
							glm::radians<float>(295.f), 
							0.f)
					)
				}, 
				{},
				{},
			{
				"S"
				,1.0f
				, m_popupCol
				, Frac::TextComponent::FontFamily::Icons
				, true
			});

			const auto uiPopup = m_world.get_mutable<UIPopUp>(m_activePopupEnt);
			
			// info elements
			{
				const std::pair<std::string, std::string> infoElements[4]
				{
					{"Logo", "V"},
					{"TextHeader", ""},
					{"Health", ""},
					{"OneLineInfo", ""}
				};

				for (size_t i = 0; i < 4; i++)
				{
					const Frac::EntityHandle infoElement = m_world.makeEntity<TOR::Transform, UIElement, UIPopUp, Frac::TextComponent>(canvasName + "_Popup_" + infoElements[i].first, 
						{
							glm::vec3(0.f, static_cast<float>(static_cast<float>(i) * -5.f) + 12.f, 0.f),
							glm::vec3(0.2f)}, 
						{},
						{},
					{
						infoElements[i].second
						,0.45f
						, m_resCountColor
						, !i ? Frac::TextComponent::FontFamily::Icons : Frac::TextComponent::FontFamily::Regular
					});

					m_world.addChild(infoElement, m_activePopupEnt);
					
					uiPopup->infoElements[infoElements[i].first] = infoElement;
				}
			}
		}
	}

	return canvasName;
}

std::string UIManager::CreatePauseCanvas()
{
	std::string canvasName = "PauseCanvas";

	Frac::EntityHandle theCanvasEntity = m_world.makeEntity<TOR::Transform, UICanvas, Frac::TextComponent>(canvasName,
		{},
		{
			canvasName
		},
		{});

	UICanvas* theCanvas = m_world.get<UICanvas>(theCanvasEntity);
	m_canvasesMap.insert({canvasName, theCanvasEntity});

	{
		Frac::EntityHandle mmLogoEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_Logo", 
			{m_logoPos, m_logoSize}, 
			{}, 
			{"u" // paint icon A
			, 5.0f
			, glm::vec3(1.f)
			, Frac::TextComponent::FontFamily::Icons });

		m_world.addChild(
			mmLogoEnt,
			theCanvasEntity
		);

		Frac::EntityHandle mmBackdropEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_Backdrop", 
			{m_backdropPos, m_backdropSize} , 
			{} , 
			{"S" // square
			, m_backdropFontSize
			, m_backdropColor
			, Frac::TextComponent::FontFamily::Icons }
		);
		m_world.addChild(
			mmBackdropEnt,
			theCanvasEntity
		);

		{
			theCanvas->theSelectorEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_Selector", 
				{glm::vec3(-8.f, 0.f, m_buttonDepth), glm::vec3(0.5f)}, 
				{}, 
					{
						">"
						,0.5f
				, glm::vec3(0.25f, 1.f, 0.25f)
				, Frac::TextComponent::FontFamily::Icons
					});
			m_world.addChild(
				theCanvas->theSelectorEnt,
				mmBackdropEnt
			);


			std::vector<std::pair<std::string, std::string>> pauseItems = m_menuItems;
			pauseItems.pop_back();
			pauseItems.push_back({ "MainMenuCanvas", "Main Menu" });
			
			int yIncrement = 0;
			for (auto it = pauseItems.begin(); it != pauseItems.end(); ++it)
			{
				const std::string butName = it->second;
				
				Frac::EntityHandle mmButtonEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_" + butName + "_Button", 
					{glm::vec3(
						0.f, 
						m_buttonVerStartOffset + ( static_cast<float>(yIncrement++) * -m_buttonVerPadding), 
						m_buttonDepth), glm::vec3(0.5f)}, 
					{
						it->first
					}, 
					{
						it->second
						, m_buttonFontSize
						, theCanvas->idleColor
					});
				m_world.addChild(
					mmButtonEnt,
					mmBackdropEnt
				);

				theCanvas->selectableElements.push_back(mmButtonEnt);
			}
		}
	}

	return canvasName;
}

std::string UIManager::CreateVictoryCanvas()
{
	std::string canvasName = "VictoryCanvas";

	Frac::EntityHandle theCanvasEntity = m_world.makeEntity<TOR::Transform, UICanvas, Frac::TextComponent>(canvasName,
		{},
		{
			canvasName
		},
		{});

	UICanvas* theCanvas = m_world.get<UICanvas>(theCanvasEntity);
	m_canvasesMap.insert({canvasName, theCanvasEntity});

	{
		Frac::EntityHandle mmLogoEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_Logo", 
			{m_logoPos, m_logoSize}, 
			{}, 
			{"Victory" // paint icon A
			, 5.0f
			, m_winHeaderCol
			});

		m_world.addChild(
			mmLogoEnt,
			theCanvasEntity
		);

		Frac::EntityHandle mmBackdropEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_Backdrop", 
			{m_backdropPos, m_backdropSize} , 
			{} , 
			{"R" // Circle
			, m_winLoseBackdropSize
			, m_winBackDropCol
			, Frac::TextComponent::FontFamily::Icons }
		);
		m_world.addChild(
			mmBackdropEnt,
			theCanvasEntity
		);

		// information
		{
			Frac::EntityHandle infoBox = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_BuildInfo", 
				{glm::vec3(0.f, 10.f, m_buttonDepth), glm::vec3(0.25f)}, 
				{}, 
				{
					"Try again?"
					,1.0f
				, theCanvas->idleColor
				});
			m_world.addChild(
				infoBox,
				mmBackdropEnt
			);
		}

		// selectable elements + selector
		{
			theCanvas->theSelectorEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_Selector", 
				{glm::vec3(-8.f, 0.f, m_buttonDepth), glm::vec3(0.5f)}, 
				{}, 
					{
						">"
						,0.5f
				, glm::vec3(0.25f, 1.f, 0.25f)
				, Frac::TextComponent::FontFamily::Icons
					});
			m_world.addChild(
				theCanvas->theSelectorEnt,
				mmBackdropEnt
			);

			const std::string uiButtons[3] = {
				"Try Again",
				"Next Level",
				"Main Menu"
			};

			for (size_t i = 0; i < 3; i++)
			{
				Frac::EntityHandle mmButtonRestartEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_" + uiButtons[i] + "Button", 
					{glm::vec3(
						0.f, 
						m_winLoseButtonVerOffset - (15.f * i), 
						m_buttonDepth), glm::vec3(0.5f)}, 
					{}, 
				{
					uiButtons[i]
					,m_buttonFontSize - 0.5f
					, theCanvas->idleColor
				});
				m_world.addChild(
					mmButtonRestartEnt,
					mmBackdropEnt
				);
				
				theCanvas->selectableElements.push_back(mmButtonRestartEnt);
			}
		}
	}

	return canvasName;
}

std::string UIManager::CreateDefeatCanvas()
{
	std::string canvasName = "DefeatCanvas";

	Frac::EntityHandle theCanvasEntity = m_world.makeEntity<TOR::Transform, UICanvas, Frac::TextComponent>(canvasName,
		{},
		{
			canvasName
		},
		{});

	UICanvas* theCanvas = m_world.get<UICanvas>(theCanvasEntity);
	m_canvasesMap.insert({canvasName, theCanvasEntity});

	{
		Frac::EntityHandle mmLogoEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_Logo", 
			{m_logoPos, m_logoSize}, 
			{}, 
			{"Defeat" // paint icon A
			, 5.0f
			, m_loseHeaderCol
			 });

		m_world.addChild(
			mmLogoEnt,
			theCanvasEntity
		);

		Frac::EntityHandle mmBackdropEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_Backdrop", 
			{m_backdropPos, m_backdropSize} , 
			{} , 
			{"R" // Circle
			, m_winLoseBackdropSize
			, m_loseBackDropCol
			, Frac::TextComponent::FontFamily::Icons }
		);
		m_world.addChild(
			mmBackdropEnt,
			theCanvasEntity
		);

		// information
		{
			Frac::EntityHandle infoBox = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_BuildInfo", 
				{glm::vec3(0.f, 10.f, m_buttonDepth), glm::vec3(0.25f)}, 
				{}, 
				{
					"Try again?"
					,1.0f
				, theCanvas->idleColor
				});
			m_world.addChild(
				infoBox,
				mmBackdropEnt
			);
		}

		// selectable elements + selector
		{
			theCanvas->theSelectorEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_Selector", 
				{glm::vec3(-8.f, 0.f, m_buttonDepth), glm::vec3(0.5f)}, 
				{}, 
					{
						">"
						,0.5f
				, glm::vec3(0.25f, 1.f, 0.25f)
				, Frac::TextComponent::FontFamily::Icons
					});
			m_world.addChild(
				theCanvas->theSelectorEnt,
				mmBackdropEnt
			);
			
			const std::string uiButtons[2] = {
				"Try Again",
				"Main Menu"
			};

			for (size_t i = 0; i < 2; i++)
			{
				Frac::EntityHandle buttonElement = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_" + uiButtons[i] + "Button", 
					{glm::vec3(
						0.f, 
						m_winLoseButtonVerOffset - (15.f * i), 
						m_buttonDepth), glm::vec3(0.5f)}, 
					{}, 
				{
					uiButtons[i]
					,m_buttonFontSize - 0.5f
					, theCanvas->idleColor
				});
				m_world.addChild(
					buttonElement,
					mmBackdropEnt
				);

				theCanvas->selectableElements.push_back(buttonElement);
			}
		}
	}

	return canvasName;
}

std::string UIManager::CreateControlsCanvas()
{
	std::string canvasName = "ControlsCanvas";
	
	Frac::EntityHandle theCanvasEntity = m_world.makeEntity<TOR::Transform, UICanvas, Frac::TextComponent>(canvasName,
		{},
		{
			canvasName
		},
		{});
	
	UICanvas* theCanvas = m_world.get<UICanvas>(theCanvasEntity);
	m_canvasesMap.insert({canvasName, theCanvasEntity});
	
	{
		Frac::EntityHandle mmLogoEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_Logo", 
			{m_logoPos, m_logoSize}, 
			{}, 
			{"8" // paint icon A
			, 5.0f
			, glm::vec3(1.f)
			, Frac::TextComponent::FontFamily::Icons });

		m_world.addChild(
			mmLogoEnt,
			theCanvasEntity
		);

		Frac::EntityHandle mmBackdropEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_Backdrop", 
			{m_backdropPos, m_backdropSize} , 
			{} , 
			{"S" // square
			, m_backdropFontSize
			, m_backdropColor
			, Frac::TextComponent::FontFamily::Icons }
		);
		m_world.addChild(
			mmBackdropEnt,
			theCanvasEntity
		);

		// information
		{
			Frac::EntityHandle buildInfo = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_BuildInfo", 
				{glm::vec3(0.f, 5.f, m_buttonDepth), glm::vec3(0.5f)}, 
				{}, 
				{
					"E to build"
					,0.5f
					, theCanvas->idleColor
				});
			m_world.addChild(
				buildInfo,
				mmBackdropEnt
			);

			Frac::EntityHandle sellInfo = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_BuildInfo", 
				{glm::vec3(0.f, -5, m_buttonDepth), glm::vec3(0.5f)}, 
				{}, 
				{
					"Q to sell"
					,0.5f
					, theCanvas->idleColor
				});
			m_world.addChild(
				sellInfo,
				mmBackdropEnt
			);
		}
		
		// selectable elements + selector
		{
			theCanvas->theSelectorEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_Selector", 
				{glm::vec3(-8.f, 0.f, m_buttonDepth), glm::vec3(0.5f)}, 
				{}, 
					{
						">"
						,0.5f
				, glm::vec3(0.25f, 1.f, 0.25f)
				, Frac::TextComponent::FontFamily::Icons
					});
			m_world.addChild(
				theCanvas->theSelectorEnt,
				mmBackdropEnt
			);

			Frac::EntityHandle mmButtonEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_BackButton", 
				{glm::vec3(
					0.f, 
					m_controlsButtonVerOffset, 
					m_buttonDepth), glm::vec3(0.5f)}, 
				{}, 
				{
					"Back"
					,m_buttonFontSize
					, theCanvas->idleColor
				});
			m_world.addChild(
				mmButtonEnt,
				mmBackdropEnt
			);

			theCanvas->selectableElements.push_back(mmButtonEnt);
		}
	}

	return canvasName;
}

std::string UIManager::CreateOptionsCanvas()
{
	std::string canvasName = "OptionsCanvas";
	
	Frac::EntityHandle theCanvasEntity = m_world.makeEntity<TOR::Transform, UICanvas, Frac::TextComponent>(canvasName,
		{},
		{
			canvasName
		},
		{});
	
	UICanvas* theCanvas = m_world.get<UICanvas>(theCanvasEntity);
	m_canvasesMap.insert({canvasName, theCanvasEntity});

	{
		Frac::EntityHandle mmLogoEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_Logo", 
			{m_logoPos, m_logoSize}, 
			{}, 
			{"gVL" // paint icon A
			, 5.0f
			, glm::vec3(1.f)
			, Frac::TextComponent::FontFamily::Icons });

		m_world.addChild(
			mmLogoEnt,
			theCanvasEntity
		);

		Frac::EntityHandle mmBackdropEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_Backdrop", 
			{m_backdropPos, m_backdropSize} , 
			{} , 
			{"S" // square
			, m_backdropFontSize
			, m_backdropColor
			, Frac::TextComponent::FontFamily::Icons }
		);
		m_world.addChild(
			mmBackdropEnt,
			theCanvasEntity
		);

		// information
		{
			Frac::EntityHandle controlsInfoBarEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_TestInfoBar", 
				{glm::vec3(0.f, 0.f, m_buttonDepth), glm::vec3(0.5f)}, 
				{}, 
				{
					"Hello World!"
					,0.5f
				, theCanvas->idleColor
				});
			m_world.addChild(
				controlsInfoBarEnt,
				mmBackdropEnt
			);
		}

		// selectable elements + selector
		{
			theCanvas->theSelectorEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_Selector", 
				{glm::vec3(-8.f, 0.f, m_buttonDepth), glm::vec3(0.5f)}, 
				{}, 
					{
						">"
						,0.5f
				, glm::vec3(0.25f, 1.f, 0.25f)
				, Frac::TextComponent::FontFamily::Icons
					});
			m_world.addChild(
				theCanvas->theSelectorEnt,
				mmBackdropEnt
			);

			Frac::EntityHandle mmButtonEnt = m_world.makeEntity<TOR::Transform, UIElement, Frac::TextComponent>(canvasName + "_BackButton", 
				{glm::vec3(
					0.f, 
					m_controlsButtonVerOffset, 
					m_buttonDepth), glm::vec3(0.5f)}, 
				{}, 
				{
					"Back"
					,m_buttonFontSize
				, theCanvas->idleColor
				});
			m_world.addChild(
				mmButtonEnt,
				mmBackdropEnt
			);

			theCanvas->selectableElements.push_back(mmButtonEnt);
		}
	}
	
	return canvasName;
}

void UIManager::SetInitialActiveCanvas()
{
	// make sure none of them are active
	for (auto it = m_canvasesMap.begin(); it != m_canvasesMap.end(); ++it)
	{
		auto canvas = m_world.get<UICanvas>(it->second);
		canvas->isActive = false;
	}

	// ???? doesn't look nice
	switch ((*m_gameState)) {
		case GameState::StateMainMenu:
			m_world.get<UICanvas>(m_canvasesMap["MainMenuCanvas"])->isActive = true;
			break;
		case GameState::StatePauseMenu: 
			m_world.get<UICanvas>(m_canvasesMap["PauseCanvas"])->isActive = true;
			break;
		case GameState::StatePlaying: 
			m_world.get<UICanvas>(m_canvasesMap["GameplayCanvas"])->isActive = true;
			break;
		case GameState::StateVictory: 
			m_world.get<UICanvas>(m_canvasesMap["VictoryCanvas"])->isActive = true;
			break;
		case GameState::StateDefeat: 
			m_world.get<UICanvas>(m_canvasesMap["DefeatCanvas"])->isActive = true;
			break;
		case GameState::StateNone: break;
		case GameState::StateQuitGame: break;
		default: ;
	}
}

void UIManager::UpdateCanvases(EntityHandle entity, TOR::Transform& transform, UICanvas& canvas, Frac::TextComponent& text)
{
	if (m_initialRun)
	{
		SetInitialActiveCanvas();
		m_initialRun = false;
		return;
	}
	
	if (!canvas.isActive)
	{
		transform.Scale = glm::vec3(0.f);
		return;
	}

	if (m_ignoreFirst)
	{
		m_ignoreFirst = false;
		return;
	}

	if ((*m_gameState) == GameState::StateQuitGame)
	{
		return;
	}

	if ((*m_gameState) == GameState::StateDefeat || (*m_gameState) == GameState::StateVictory)
	{
		SetInitialActiveCanvas();
		//return;
	}
	
	transform.Scale = glm::vec3(1.f);
	
	UpdateCanvasTransform(transform, text);
	HandleInput();
	
	const std::string canvasName = canvas.name;

	// eww, make this dynamic
	if (canvasName == "MainMenuCanvas")
	{
		UpdateMainMenuCanvas(entity, canvas);
	}
	else if (canvasName == "GameplayCanvas")
	{
		UpdateGameplayCanvas(entity, canvas);
	}
	else if (canvasName == "PauseCanvas")
	{
		UpdatePauseCanvas(entity, canvas);
	}
	else if (canvasName == "ControlsCanvas")
	{
		UpdateControlsCanvas(entity, canvas);
	}
	else if (canvasName == "OptionsCanvas")
	{
		UpdateOptionsCanvas(entity, canvas);
	}
	else if (canvasName == "VictoryCanvas")
	{
		UpdateVictoryCanvas(entity, canvas);
	}
	else if (canvasName == "DefeatCanvas")
	{
		UpdateDefeatCanvas(entity, canvas);
	}
	else
	{
		LOGWARNING("UIManager | No known canvas is active at the moment!");
	}
}

void UIManager::UpdateMainMenuCanvas(EntityHandle entity, UICanvas& canvas)
{
	if (!canvas.theSelectorEnt || canvas.selectableElements.empty())
	{
		return;
	}
	
	UpdateCanvasSelector(canvas);

	if (m_select)
	{
		const auto uiEventName = m_world.get<UIElement>(canvas.selectableElements[canvas.curSelectedElement])->eventName;

		if (!uiEventName.empty())
		{
			if (uiEventName == "QuitGame")
			{
				SetGameState(uiEventName);
				
			}
			else
			{
				SetActiveCanvas(
					entity, 
					m_canvasesMap.at(uiEventName));
			}
		}
		else
		{
			LOGWARNING("UIManager | MainMenuCanvas | uiEventName is empty, unable to set active canvas!");
		}
	}
}

void UIManager::UpdateGameplayCanvas(EntityHandle entity, UICanvas& canvas)
{
	// update counters
	auto resourceText = m_world.get<Frac::TextComponent>(canvas.settableElements.at("ResourceCounter_0"));
	resourceText->text = std::to_string(m_gameManager.m_playerManager->GetResources());

	auto waveText = m_world.get<Frac::TextComponent>(canvas.settableElements.at("WaveCounter_0"));
	waveText->text = std::to_string(m_gameManager.m_enemyManager->m_curWave) + "/" + std::to_string(m_gameManager.m_enemyManager->m_maxWave);
	
	auto enemiesText = m_world.get<Frac::TextComponent>(canvas.settableElements.at("IncomingEnemiesCounter_0"));
	enemiesText->text = std::to_string(m_gameManager.m_enemyManager->m_totalEnemiesSpawned) + "/" + std::to_string(m_gameManager.m_enemyManager->m_totalEnemiesToSpawn);
	
	auto baseHpText = m_world.get<Frac::TextComponent>(canvas.settableElements.at("BaseHPCounter_0"));

	const Frac::EntityHandle baseEnt = m_gameManager.m_tileManager->m_tileMap->GetTileHandle(
		m_gameManager.m_playerEntityCreator->GetBasePos());
	
	if (m_world.has<TileComponent>(baseEnt))
	{
		const Frac::EntityHandle tileOcupant = m_world.get<TileComponent>(baseEnt)->m_occupier;
		if (tileOcupant != 0 && m_world.has<HealthComponent>(tileOcupant))
		{
			const auto healthComp = m_world.get<HealthComponent>(tileOcupant);
			baseHpText->text = std::to_string(healthComp->m_health);
		}
		else
		{
			baseHpText->text = "TBA";
		}
	}
	else
	{
		baseHpText->text = "TBA";
	}
	
	// handle 3D cursor
	if (m_activeCamera != 0)
	{
		if (m_cursor3DManager->IsHidden())
		{
			m_cursor3DManager->Show();
		}
		
		auto cameraLookAt = m_world.get<CameraManager::CameraTrait>(m_activeCamera)->LookAt;
		m_cursor3DManager->Update3DMouseCursor(cameraLookAt);
	}
	else
	{
		LOGWARNING("UIManager | GameplayCanvas Update | Unable to update 3D cursor, no active camera set!");
	}

	// handle input, relies on game manager to pause the game
	if (m_pause)
	{
		SetActiveCanvas(entity, m_canvasesMap["PauseCanvas"]);

		m_cursor3DManager->Hide();
	}
}

void UIManager::UpdatePauseCanvas(EntityHandle entity, UICanvas& canvas)
{
	if (!canvas.theSelectorEnt || canvas.selectableElements.empty())
	{
		return;
	}
	
	UpdateCanvasSelector(canvas);

	if (m_select)
	{
		const auto uiEventName = m_world.get<UIElement>(canvas.selectableElements[canvas.curSelectedElement])->eventName;
		
		if (!uiEventName.empty())
		{
			if (uiEventName == "QuitGame")
			{
				SetGameState(uiEventName);
			}
			else
			{
				SetActiveCanvas(
					entity, 
					m_canvasesMap.at(uiEventName));
			}
		}
		else
		{
			LOGWARNING("UIManager | PauseCanvas select, uiEventName is empty");
		}

		return;
	}

	if (m_pause)
	{
		SetActiveCanvas(
			entity, 
			m_canvasesMap.at("GameplayCanvas"));
	}
}

void UIManager::UpdateControlsCanvas(EntityHandle entity, UICanvas& canvas)
{
	UpdateCanvasSelector(canvas);
	
	if (m_select && m_lastActiveCanvasEnt != 0)
	{
		SetActiveCanvas(entity, m_lastActiveCanvasEnt);
	}
}

void UIManager::UpdateOptionsCanvas(EntityHandle entity, UICanvas& canvas)
{
	UpdateCanvasSelector(canvas);
	
	if (m_select && m_lastActiveCanvasEnt != 0)
	{
		SetActiveCanvas(entity, m_lastActiveCanvasEnt);
	}
}

void UIManager::UpdateVictoryCanvas(EntityHandle entity, UICanvas& canvas)
{
	UpdateCanvasSelector(canvas);
	
	if (m_select)
	{
		// add others
		if (canvas.curSelectedElement == 2)
		{
			SetActiveCanvas(entity, m_canvasesMap["MainMenuCanvas"]);
		}
	}
}

void UIManager::UpdateDefeatCanvas(EntityHandle entity, UICanvas& canvas)
{
	UpdateCanvasSelector(canvas);
	
	if (m_select)
	{
		// add others
		if (canvas.curSelectedElement == 1)
		{
			SetActiveCanvas(entity, m_canvasesMap["MainMenuCanvas"]);
		}
	}

}

void UIManager::UpdateCanvasSelector(UICanvas& canvas)
{
	Frac::EntityHandle lastSelectedEnt = canvas.selectableElements[canvas.lastSelectedElement];
	if (lastSelectedEnt)
	{
		Frac::TextComponent* lastTextComp = m_world.get<Frac::TextComponent>(lastSelectedEnt);
		lastTextComp->color = canvas.idleColor;
	}

	if (m_up)
	{
		canvas.curSelectedElement--;
	}
	if (m_down)
	{
		canvas.curSelectedElement++;
	}

	if (canvas.curSelectedElement < 0)
	{
		canvas.curSelectedElement = canvas.selectableElements.size() - 1;
	} else if (canvas.curSelectedElement > canvas.selectableElements.size() - 1)
	{
		canvas.curSelectedElement = 0;
	}

	canvas.lastSelectedElement = canvas.curSelectedElement;

	// selector animation
	TOR::Transform* selectorTrans				= m_world.get<TOR::Transform>(canvas.theSelectorEnt);

	TOR::Transform* selectedElementTrans		= m_world.get<TOR::Transform>(canvas.selectableElements[canvas.curSelectedElement]);
	Frac::TextComponent* selectedElementText	= m_world.get<Frac::TextComponent>(canvas.selectableElements[canvas.curSelectedElement]);

	selectedElementText->color = canvas.hoverColor;

	canvas.selectorAnimTime += m_world.getDeltaTime();
	canvas.selectorAnimCurrent = glm::cos(canvas.selectorAnimTime * canvas.selectorAnimSpeed) * canvas.selectorAnimBounce;

	selectorTrans->Position = selectedElementTrans->Position
		+ glm::vec3(65.f, 0.f, 0.f)
		+ glm::vec3(canvas.selectorAnimCurrent, 0.f, 0.f);
}

void UIManager::UpdateCanvasTransform(TOR::Transform& transform, Frac::TextComponent& text)
{
	m_activeCamera = GetActiveCamera();
	if (!m_activeCamera 
		|| !m_world.has<TOR::Transform>(m_activeCamera) 
		|| !m_world.has<CameraManager::CameraTrait>(m_activeCamera))
	{
		return;
	}
	
	text.faceActiveCamera = true;

	const TOR::Transform* camTrans				= m_world.get<TOR::Transform>(m_activeCamera);
	const CameraManager::CameraTrait* camTrait	= m_world.get<CameraManager::CameraTrait>(m_activeCamera);

	const glm::vec3 pointInfrontOfCam = camTrans->Position + 
		(glm::normalize(camTrait->LookAt - camTrans->Position) * m_canvasDepthOffset);

	transform.Position = pointInfrontOfCam;
}

void UIManager::HandleInput()
{
	m_select	= m_inputSystem.GetAnyJoystickButtonOnce(Frac::JOYSTICK_BUTTON_A)
					|| m_inputSystem.GetAnyJoystickButtonOnce(Frac::JOYSTICK_BUTTON_START);

	m_up		= m_inputSystem.GetAnyJoystickButtonOnce(Frac::JOYSTICK_BUTTON_DPAD_UP)
					|| m_inputSystem.GetAnyUp();
	m_down		= m_inputSystem.GetAnyJoystickButtonOnce(Frac::JOYSTICK_BUTTON_DPAD_DOWN)
					|| m_inputSystem.GetAnyDown();
	
	m_pause		= m_inputSystem.GetAnyJoystickButtonOnce(Frac::JOYSTICK_BUTTON_START)
					|| m_inputSystem.GetKeyOnce(Frac::KEY_ESCAPE);
}

void UIManager::HandlePopupEntity(Frac::EntityHandle target)
{
	const std::string entName = m_world.getName(target);

	const auto popupTransform = m_world.get<TOR::Transform>(m_activePopupEnt);
	const auto targetTransform = m_world.get<TOR::Transform>(target);

	popupTransform->Position = targetTransform->Position + m_popupOffset;
	
	const auto uiPopup = m_world.get<UIPopUp>(m_activePopupEnt);
	
	// from here
	const auto textHeader = m_world.get<Frac::TextComponent>(uiPopup->infoElements["TextHeader"]);
	textHeader->text = entName.empty() ? "Unnamed" : entName;
	
	auto healthText = m_world.get<Frac::TextComponent>(uiPopup->infoElements["Health"]);
	if(m_world.has<HealthComponent>(target))
	{
		const auto health = m_world.get<HealthComponent>(target);

		healthText->text = "HP:" + std::to_string(health->m_health);
	}
	else
	{
		healthText->text = "";
	}
	
	auto oneLineInfoText = m_world.get<Frac::TextComponent>(uiPopup->infoElements["OneLineInfo"]);
	if(m_world.has<TurretComponent>(target))
	{
#ifdef PLATFORM_WINDOWS
		oneLineInfoText->text = "Q to sell";
#else
		oneLineInfoText->text = "A/o to sell";
#endif
	}
	else
	{
		oneLineInfoText->text = "";
	}

	if (m_world.has<SpawnerComponent>(target))
	{
		textHeader->text = "Spawner";
	}

	if (m_world.has<TurretComponent>(target))
	{
		textHeader->text = "Turret";
	}
}

void UIManager::SetActiveCanvas(Frac::EntityHandle toDeactive, Frac::EntityHandle toActive)
{
	const auto toDeactivateCanvas	= m_world.get<UICanvas>(toDeactive);
	const auto toActiveCanvas		= m_world.get<UICanvas>(toActive);

	toDeactivateCanvas->isActive	= false;
	toActiveCanvas->isActive		= true;
	
	m_lastActiveCanvasEnt			= toDeactive;

	m_ignoreFirst = true;
	
	SetGameState(toActiveCanvas->name);
}

void UIManager::SetGameState(const std::string& stringToCheck)
{
	GameState newGameState = (*m_gameState);

	// pain
	if (stringToCheck == "MainMenuCanvas")
	{
		newGameState = GameState::StateMainMenu;
	}
	else if (stringToCheck == "GameplayCanvas")
	{
		newGameState = GameState::StatePlaying;
	}
	else if (stringToCheck == "PauseCanvas")
	{
		newGameState = GameState::StatePauseMenu;
	}
	else if (stringToCheck == "VictoryCanvas")
	{
		newGameState = GameState::StateVictory;
	}
	else if (stringToCheck == "DefeatCanvas")
	{
		newGameState = GameState::StateDefeat;
	}
	else if (stringToCheck == "QuitGame")
	{
		newGameState = GameState::StateQuitGame;
	}
	
	(*m_gameState) = newGameState;
}

Frac::EntityHandle UIManager::GetActiveCamera()
{
	Frac::EntityHandle activeCam = 0;
	std::function<void(Frac::It, uint64_t, TOR::Camera*)> func = [&activeCam](Frac::It it, uint64_t index, TOR::Camera* camera) mutable
	{
		if(camera[index].IsActive)
		{
			activeCam = it.m_it.entity(index).id();
		}
	};
	m_camEntsQuery.Iter(func);

	return activeCam;
}
