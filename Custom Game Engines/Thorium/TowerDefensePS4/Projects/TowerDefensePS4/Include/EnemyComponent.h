#pragma once
#include <glm/glm.hpp>
#include "Tile.h"
namespace Th {
	namespace Components {
		struct TransformComponent;
	}
}

struct EnemyComponent
{
public:
	EnemyComponent();
	~EnemyComponent() = default;

	void Initialize(float speed, int health, int gold);

	bool MoveTo(glm::vec3 pos,Th::Components::TransformComponent transform);
	bool MoveToTarget(glm::vec3 target, Th::Components::TransformComponent* transform, float dt);
	std::vector<Tile*>* m_ActivePath;
	float GetSpeed();
	int GetHealth();
	void ChangeHealth(int value);
	glm::vec3 GetMoveTo();
	int GetWorth();

	int m_ActiveTargetTileIndex = 0;
private:
	float m_Speed;
	int m_Health;
	int m_GoldWorth;
	glm::vec3 moveto = glm::vec3(2, 5, 4);
	glm::vec3 next = glm::vec3(8, 5, 4);
};

