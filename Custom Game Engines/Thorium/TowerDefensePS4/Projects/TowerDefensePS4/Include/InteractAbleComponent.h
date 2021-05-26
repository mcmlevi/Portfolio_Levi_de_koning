#pragma once
#include <entt/single_include/entt/entt.hpp>
#include <glm/glm.hpp>

namespace Th {
	namespace Components {
		struct TransformComponent;
	}
}

struct InteractAbleComponent {
public:
	bool m_Initialized = false;
	InteractAbleComponent();
	~InteractAbleComponent();
	void SetInteractableType(int type);
	enum InteractableType {
		Tree,
		Grass,
		Obstacle,
		Rock,
		Checkpoint,
		BeginEndPoint,
	};

	int m_CurrentInteractableType;
private:
};