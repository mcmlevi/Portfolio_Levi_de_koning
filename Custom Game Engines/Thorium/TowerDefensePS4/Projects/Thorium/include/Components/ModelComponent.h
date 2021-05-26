#pragma once
#include <vector>
#include <Resources/Mesh.h>
#include <Resources/Shader.h>
#include <Components/BoxColliderComponent.h>
namespace Th
{
	/// <summary>
	/// The render component contains classes that are only required for rendering. 
	/// </summary>
	struct RenderComponent
	{
		Shader RenderShader;
		Mesh RenderMesh;
	};

	namespace Components
	{
		/// <summary>
		/// Universal model data supporting cross platform use.
		/// </summary>
		struct ModelComponent
		{
		public:
			ModelComponent() = default;
			~ModelComponent();

			RenderComponent& operator[](const int index);

			void SetMeshes(const std::vector<RenderComponent>& meshes);
			void SetCollider(glm::vec3 min, glm::vec3 max);
			BoxColliderComponent getCollider();
			int GetMeshCount() const;

		private:
			std::vector<RenderComponent> m_Renderables;
			BoxColliderComponent m_Collider;
		};
	}
}