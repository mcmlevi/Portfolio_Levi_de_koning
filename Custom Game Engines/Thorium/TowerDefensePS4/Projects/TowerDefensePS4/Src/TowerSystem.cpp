#include "pch.h"
#include "TowerSystem.h"
#include "TowerComponent.h"

#include <utility>
#include <Core/MessageLogger.h>
#include <Core/ResourceManager.h>
#include <Core/WorldManager.h>

#include <Components/ModelComponent.h>
#include <Resources/Shader.h>

#include "EnemyComponent.h"
#include "ProjectileComponent.h"
#include "Events/InputHandler.h"
#include "Events/EventArgs.h"

#include "Core/Audio/AudioManager.h"


TowerSystem::TowerSystem()
{
	Th::InputHandler::getInstance().GetKeyPresedEventDelegate().Connect(&TowerSystem::KeyPressed, this);
	Th::InputHandler::getInstance().GetKeyReleasedEventDelegate().Connect(&TowerSystem::KeyReleased, this);
	//register to the update
	Register();

	m_TowerPlacedWithKey = false;
	//CreateTower(TargetMode::Close, 1, 0.1f, 5.0f, glm::vec3(5, 5, 5), 2.5f,test );
	//CreateTower(TargetMode::Strong, 1, 0.1f, 5.0f, glm::vec3(4, 5, 5), 2.5f, test);
}

void TowerSystem::Initialize(LevelManager* levelManager, PlayerSystem* playerSystem) {
	m_LevelManager = levelManager;
	m_PlayerSystem = playerSystem;
}

void TowerSystem::CreateTower(TargetMode mode, int damage, float attackspeed, float rotationspeed, Tile* selectedTile, float range, std::vector<glm::vec3> spawnpoints)
{
	if (!m_TowerPlacedWithKey) {
		m_TowerPlacedWithKey = true;
		if (selectedTile != nullptr) {
			if (m_PlayerSystem->GetGold() > 0) {
				//Ease for syntax, do *never* store as member variables.
				Th::ResourceManager& resourceManager = Th::ResourceManager::GetInstance();
				Th::WorldManager& worldManager = Th::WorldManager::GetSingleton();

				//creating a reference to the tranform component of the selected tile.
				auto& transform = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(selectedTile->m_TileEntity);
				selectedTile->m_TileTowerConnection = entt::entity(worldManager.CreateEntity("TowerTileConnection", transform));
				selectedTile->m_TowerBase = entt::entity(worldManager.CreateEntity("ToweBase", transform));
				selectedTile->m_TowerHead = entt::entity(worldManager.CreateEntity("TowerHead", transform));
				Th::ModelComponent* TileConnection = m_AllTileTowerModels[selectedTile->currentTileType];
				Th::ModelComponent* MiddlePart = resourceManager.GetModel("[models]/PlayerTowers/MiddlePart.obj");
				Th::ModelComponent* HeadModel = resourceManager.GetModel("[models]/PlayerTowers/CanonHead.obj");
				worldManager.GetEntityRegister().emplace<ModelComponent>(selectedTile->m_TileTowerConnection, *m_AllTileTowerModels[selectedTile->currentTileType]);
				worldManager.GetEntityRegister().emplace<ModelComponent>(selectedTile->m_TowerBase, *m_AllTileTowerModels[5]);
				worldManager.GetEntityRegister().emplace<ModelComponent>(selectedTile->m_TowerHead, *m_AllTileTowerModels[4]);

				auto& BaseTransform = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(selectedTile->m_TowerBase);
				auto& HeadTransform = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(selectedTile->m_TowerHead);
				glm::vec3 basePos = BaseTransform.GetPosition();
				glm::vec3 headPos = HeadTransform.GetPosition();
				
				basePos.y += GetTowerHeightOffset(selectedTile->currentTileType, false);
				headPos.y += GetTowerHeightOffset(selectedTile->currentTileType, true);

				BaseTransform.SetPosition(basePos);
				HeadTransform.SetPosition(headPos);

				TowerComponent TowerComp;
				TowerComp.Initialize(mode, damage, attackspeed, rotationspeed, range, std::move(spawnpoints));
				worldManager.GetEntityRegister().emplace<TowerComponent>(selectedTile->m_TowerHead, TowerComp);

				m_Towers.push_back(selectedTile->m_TowerHead);
				m_LevelManager->m_SelectedTile->m_Passable = false;
				m_LevelManager->m_SelectedTile = nullptr;

				auto& ArrowTransform = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(m_LevelManager->m_LevelArrow);
				ArrowTransform.SetPosition(glm::vec3(0, 1000, 0));
				m_LevelManager->CalculateFullPath();
			}
		}
	}
}

void TowerSystem::OnUpdate(float dt)
{

	for (auto entity : GetEntityRegister().view<TowerComponent>())
	{
		TowerComponent& tower = GetEntityRegister().get<TowerComponent>(entity);
		Th::TransformComponent& transform = GetEntityRegister().get<Th::TransformComponent>(entity);
		glm::vec3 pos = transform.GetPosition();

		std::vector<glm::vec3> newspawnpos;
		int index = tower.index;

		glm::vec3 right = transform.GetRight();
		const float forwardOffset = 0.53f * transform.GetScale().z;
		const float height = 0 * transform.GetScale().y;
		const float rightOffset = 0.08f * transform.GetScale().x;

		glm::vec3 forward = transform.GetForward() * forwardOffset;
		glm::vec3 offsetheight = glm::vec3(0, height, 0);
		glm::vec3 offsetRight = right * rightOffset;

		newspawnpos.emplace_back(glm::vec3(pos.x + forward.x + offsetRight.x, pos.y + offsetheight.y, pos.z + forward.z + offsetRight.z));
		newspawnpos.emplace_back(glm::vec3(pos.x + forward.x - offsetRight.x, pos.y + offsetheight.y, pos.z + forward.z - offsetRight.z));

		float Distance = INFINITY;
		int maxHealth = 0;
		entt::entity target = entt::null;

		//Checking if enemies are in range or not
		for (auto entity2 : GetEntityRegister().view<EnemyComponent>())
		{
			EnemyComponent& enemy = GetEntityRegister().get<EnemyComponent>(entity2);
			Th::TransformComponent& transform2 = GetEntityRegister().get<Th::TransformComponent>(entity2);
			glm::vec3 pos2 = transform2.GetPosition();

			const float distanceToEnemy = glm::distance(pos, pos2);

			float range = tower.GetRange();

			if (distanceToEnemy <= range)
			{

				tower.AddEnemyToNearestList(entity2);
			}
			else
			{
				tower.RemoveEnemyFromNearbyList(entity2);
			}

			const int healthE = enemy.GetHealth();

			switch (tower.GetTargetMode())
			{
			case TargetMode::First:
				//waiting for the pathfinding
				break;
			case TargetMode::Last:
				//waiting for the pathfinding
				break;
			case TargetMode::Strong:
				if (healthE > maxHealth && distanceToEnemy < range)
				{
					maxHealth = healthE;
					tower.SetTarget(entity2);
				}
				break;
			case TargetMode::Close:
				if (distanceToEnemy < Distance)
				{
					Distance = distanceToEnemy;
					tower.SetTarget(entity2);
				}

				break;
			default:;
			}
		}

		std::vector<entt::entity> list = tower.GetEnemiesNearList();
		if (!list.empty())
		{
			target = tower.GetTarget();
			Th::TransformComponent& targettrans = GetEntityRegister().get<Th::TransformComponent>(target);

			glm::vec3 posTarget = targettrans.GetPosition();
			glm::quat lookAtRot = LookRotationQuat(glm::vec3(pos.x, pos.y, pos.z), glm::vec3(posTarget.x, posTarget.y, posTarget.z), transform.GetUp());

			glm::quat newRotation;
			newRotation = glm::slerp(transform.GetRotation(), lookAtRot, tower.GetRotationSpeed() * dt);

			transform.SetRotation(newRotation);

			tower.m_Timer += dt;
			if (tower.GetAttackSpeed() < tower.m_Timer)
			{
				tower.index++;
				if (tower.index > newspawnpos.size() - 1)
				{
					tower.index = 0;
				}

				tower.m_Timer -= tower.GetAttackSpeed();
				glm::vec3 spawn = newspawnpos[index];
				CreateProjectile(spawn,tower.GetDamage(),target);
			}
		}
	}
}

glm::vec3 TowerSystem::FindLookAtRotation(Th::TransformComponent tower, Th::TransformComponent target, glm::vec3 up)
{
	glm::vec3 direction = glm::normalize(target.GetPosition() - tower.GetPosition());
	glm::vec3 orientation = glm::eulerAngles(glm::quatLookAtLH(direction, tower.GetUp()));
	return orientation;
}

glm::quat TowerSystem::LookRotationQuat(Th::Components::TransformComponent tower,
	Th::Components::TransformComponent target, glm::vec3 up)
{
	glm::vec3 direction = glm::normalize(target.GetPosition() - tower.GetPosition());
	glm::quat lookAtRot = glm::quatLookAtLH(direction, tower.GetUp());
	return lookAtRot;
}

float TowerSystem::GetTowerHeightOffset(int TileType, bool IsHead) {
	switch (TileType) {
	case 0:
		if (IsHead) return 1.0;
		return 0;
		break;
	case 1:
		if (IsHead) return 1.17251;
			return 0.17251;
		break;
	case 2:
		if (IsHead) return 1.2565;
		return 0.256504;
		break;
	case 3:
		if (IsHead) return 1.3534; 
		return 0.353396;
		break;
	default:
		break;
	}
}

glm::quat TowerSystem::Slerp(glm::quat a, glm::quat b, float time) const
{
	//create a quaternion to store the new rotation
	glm::quat r;

	float t = 1 - time;
	float Wa, Wb;

	//dot product to get the cosine of the angle
	float dot = glm::dot(a, b);

	//clamp the dotproduct because floats
	float val = 1.0f;
	dot = glm::clamp(dot, FLT_EPSILON, val);
	//a.x*b.x + a.y*b.y+a.z*b.z+a.w*b.w

	//get the angle between the 2 rotations
	float theta = acos(dot);
	if (theta <= 0.0f)
	{
		theta = -theta;
	}
	float sn = sin(theta);
	//assert(sn);
	Wa = sin(t * theta) / sn;
	Wb = sin(time * theta) / sn;

	//fill in the values on the new quaternion rotation
	r.x = Wa * a.x + Wb * b.x;
	r.y = Wa * a.y + Wb * b.y;
	r.z = Wa * a.z + Wb * b.z;
	r.w = Wa * a.w + Wb * b.w;

	return glm::normalize(r);
}


void TowerSystem::CreateProjectile(glm::vec3 spawn, unsigned int damage, entt::entity target)
{
	Th::AudioManager& audioManager = Th::AudioManager::GetInstance();
	audioManager.PlayEvent("event:/FX/fire1", spawn, 0.5f);

	Th::ResourceManager& resourceManager = Th::ResourceManager::GetInstance();
	Th::WorldManager& worldManager = Th::WorldManager::GetSingleton();

	//create projectile				
	Th::TransformComponent transformB(spawn, entt::null, glm::vec3(0), glm::vec3(0.5f));
	entt::entity bullet = worldManager.CreateEntity("Projectile", transformB);

	Th::ModelComponent* model = resourceManager.GetModel("[models]/Projectile/BulletBlack.obj");
	for (int i = 0; i < resourceManager.GetModel("[models]/Projectile/BulletBlack.obj")->GetMeshCount(); i++)
	{
		(*resourceManager.GetModel("[models]/Projectile/BulletBlack.obj"))[i].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	}
	worldManager.GetEntityRegister().emplace<Th::ModelComponent>(bullet, *model);

	ProjectileComponent projectile;
	projectile.Initialize(8, 1, damage, target);

	worldManager.GetEntityRegister().emplace<ProjectileComponent>(bullet, projectile);
}
void TowerSystem::SetTowerModels(Th::RenderCore* renderCore) {
	Th::ResourceManager& resourceManager = Th::ResourceManager::GetInstance();

	//flat tile
	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/PlayerTowers/FlatTileBase.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/PlayerTowers/FlatTileBase.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllTileTowerModels[0] = resourceManager.GetModel("[models]/PlayerTowers/FlatTileBase.obj");
	//DiagonalTile
	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/PlayerTowers/DiagonalTileBase.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/PlayerTowers/DiagonalTileBase.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllTileTowerModels[1] = resourceManager.GetModel("[models]/PlayerTowers/DiagonalTileBase.obj");
	//CornerTileDownwards
	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/PlayerTowers/CornerUpwardsBase.obj", Th::EModelFileType::OBJ); 
	(*resourceManager.GetModel("[models]/PlayerTowers/CornerUpwardsBase.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllTileTowerModels[2] = resourceManager.GetModel("[models]/PlayerTowers/CornerUpwardsBase.obj");
	//CornerTileUpward
	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/PlayerTowers/CornerDownwardsBase.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/PlayerTowers/CornerDownwardsBase.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllTileTowerModels[3] = resourceManager.GetModel("[models]/PlayerTowers/CornerDownwardsBase.obj");

	//Turret head
	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/PlayerTowers/CanonHead.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/PlayerTowers/CanonHead.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllTileTowerModels[4] = resourceManager.GetModel("[models]/PlayerTowers/CanonHead.obj");

	//Middle part
	resourceManager.LoadModel(renderCore->GetResourceLoader(), "[models]/PlayerTowers/MiddlePart.obj", Th::EModelFileType::OBJ);
	(*resourceManager.GetModel("[models]/PlayerTowers/MiddlePart.obj"))[0].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
	m_AllTileTowerModels[5] = resourceManager.GetModel("[models]/PlayerTowers/MiddlePart.obj");


}

void TowerSystem::KeyPressed(Th::KeyEventArgs& args) {
	if (args.Key == Th::KeyCode::T) {
		std::vector<glm::vec3> test;
		test.emplace_back(1, 0, 0);
		test.emplace_back(-1, 0, 0);
		if (m_TowerPlacedWithKey == false) {
			CreateTower(TargetMode::Close, 1, 0.1f, 5.0f, m_LevelManager->m_SelectedTile, 2.5f, test);
		}

	}
}

void TowerSystem::KeyReleased(Th::KeyEventArgs& args) {
	if (args.Key == Th::KeyCode::T) {
		m_TowerPlacedWithKey = false;
	}
}
