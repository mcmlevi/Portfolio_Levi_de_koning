#include "Thpch.h"
#include <Components/ModelComponent.h>

namespace Th
{
	namespace Components
	{
		ModelComponent::~ModelComponent()
		{
			m_Renderables.clear();
		}

		RenderComponent& ModelComponent::operator[](int index)
		{
			return m_Renderables[index];
		}

		void ModelComponent::SetMeshes(const std::vector<RenderComponent>& a_Meshes)
		{
			m_Renderables = a_Meshes;
		}

		void ModelComponent::SetCollider(glm::vec3 min, glm::vec3 max)
		{
			m_Collider = BoxColliderComponent{ min,max };
		}

		BoxColliderComponent ModelComponent::getCollider()
		{
			return m_Collider;
		}

		int ModelComponent::GetMeshCount() const
		{
			return static_cast<int>(m_Renderables.size());
		}
	}
}
