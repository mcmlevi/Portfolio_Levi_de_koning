#pragma once
#include "TowerComponent.h"
#include "Core\Systems.h"
#include "LevelManager.h"
#include "PlayerSystem.h"
class TowerSystem :
	public Th::Systems
{
public:
	TowerSystem();
	~TowerSystem() = default;
	void Initialize(LevelManager* levelManager, PlayerSystem* playerSystem);
	void CreateTower(TargetMode mode, int damage, float attackspeed, float rotationspeed, Tile* selectedTile, float range, std::vector<glm::vec3> spawnpoints);
	void OnUpdate(float) override;
	glm::vec3 FindLookAtRotation(Th::Components::TransformComponent tower, Th::Components::TransformComponent target, glm::vec3 up);
	glm::quat LookRotationQuat(Th::Components::TransformComponent tower, Th::Components::TransformComponent target, glm::vec3 up);
	void Shoot(entt::entity);
	float GetTowerHeightOffset(int TileType, bool IsHead);
	glm::quat Slerp(glm::quat a, glm::quat b, float time) const;
	void CreateProjectile(glm::vec3 spawn, unsigned int damage, entt::entity target);
	void SetTowerModels(Th::RenderCore* renderCore);
private:
	std::vector<entt::entity> m_Towers;
	void KeyPressed(Th::KeyEventArgs& args);
	void KeyReleased(Th::KeyEventArgs& args);
	LevelManager* m_LevelManager;
	PlayerSystem* m_PlayerSystem;
	bool m_TowerPlacedWithKey = false;
	ModelComponent* m_AllTileTowerModels[5];
};

