#pragma once
#ifdef PLATFORM_WIN64
#pragma warning(push,0)
#pragma warning(disable 4201 4834 4267 4100)
#elif PLATFORM_PS4
#endif
#include <entt/single_include/entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#ifdef PLATFORM_WIN64
#pragma warning(pop)
#elif PLATFORM_PS4
#endif
#include <vector>
namespace Th
{
	namespace Components
	{
		/// <summary>
		/// The transform component represents the position/orientation/scale in a 3D Cartesian space 
		/// defined by the parents and children of this class.
		/// </summary>
		struct TransformComponent final
		{

		public:
			TransformComponent(const glm::vec3& position = glm::vec3(0.0f), entt::entity parent = entt::null, const glm::vec3& eulerRotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));
			TransformComponent(const glm::vec3& position, const glm::vec3& targetPos, entt::entity parent = entt::null, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f), const glm::vec3& eulerRotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));
			~TransformComponent();

			// --------------------------------------------- \\
			// ----- || Position | Rotation | Scale || ----- \\
			// --------------------------------------------- \\

			/// <summary>
			/// Set the positon of the transform.
			/// </summary>
			/// <param name=""></param>
			void SetPosition(const glm::vec3& a_Position);

			/// <summary>
			/// </summary>
			/// <returns>The position of the transform.</returns>
			glm::vec3 GetPosition() const;

			/// <summary>
			/// Set the rotation of the transform using euler angles (pitch, yaw, roll).
			/// </summary>
			/// <param name=""></param>
			void SetRotation(const glm::vec3& a_Rotation);

			/// <summary>
			/// Set the rotation of the transform using quaternion.
			/// </summary>
			/// <param name=""></param>
			void SetRotation(const glm::quat& a_Rotation);

			/// <summary>
			/// </summary>
			/// <returns>The rotation in quaternion.</returns>
			glm::quat GetRotation() const;

			/// <summary>
			/// </summary>
			/// <returns>The rotation in euler angles.</returns>
			glm::vec3 GetEulerAngles() const;

			/// <summary>
			/// Set the scale of the transform.
			/// </summary>
			/// <param name=""></param>
			void SetScale(const glm::vec3& a_Scale);

			/// <summary>
			/// </summary>
			/// <returns>The scale of the transform.</returns>
			glm::vec3 GetScale() const;

			// ------------------------ \\
			// ----- || Matrix || ----- \\
			// ------------------------ \\

			/// <summary>
			/// Make the model matrix for this transform.
			/// </summary>
			void CalculateMatrices();
			/// <summary>
			/// The world matrix of this transform.
			/// </summary>
			glm::mat4 GetWorldMatrix() const;

			/// <summary>
			/// </summary>
			/// <returns>The local matrix of this transform.</returns>
			glm::mat4 GetLocalMatrix() const;

			/// <summary>
			/// </summary>
			/// <returns>The world to local matrix of this transform.</returns>
			glm::mat4 WorldToLocalMatrix();

			/// <summary>
			/// </summary>
			/// <returns>The local to world matrix of this transform.</returns>
			glm::mat4 LocalToWorldMatrix();

			// -------------------------------- \\
			// ----- || Parent | Child || ----- \\
			// -------------------------------- \\

			/// <summary>
			/// Set the parent of this transform, releases the if already exisiting parent transform.
			/// </summary>
			/// <param name=""></param>
			void SetParent(entt::entity parent, entt::entity thisEntity);

			/// <summary>
			/// </summary>
			/// <returns>The parent transform of this transform.</returns>
			entt::entity GetParent() const;

			/// <summary>
			/// Add transform as child to this transform.
			/// </summary>
			/// <param name=""></param>
			void AddChild(entt::entity);

			/// <summary>
			/// </summary>
			/// <param name=""></param>
			/// <returns>The child transdorm specified by the index, returns nullptr if index is out of range.</returns>
			entt::entity GetChild(const int) const;

			/// <summary>
			/// Remove child by reference.
			/// </summary>
			/// <param name=""></param>
			void DetachChild(entt::entity child);


			/// <summary>
			/// </summary>
			/// <returns>The amount of children attached to this transform.</returns>
			int GetChildCount();

			/// <summary>
			/// </summary>
			/// <returns>The root transform of this transform.</returns>
			entt::entity GetRoot() const;

			// --------------------------- \\
			// ----- || Utilities || ----- \\
			// --------------------------- \\
			
			/// <summary>
			/// </summary>
			/// <returns>The forward axis of the transform in world space.</returns>
			glm::vec3 GetForward() const;

			/// <summary>
			/// </summary>
			/// <returns>The up axis of the transform in world space.</returns>
			glm::vec3 GetUp() const;

			/// <summary>
			/// </summary>
			/// <returns>The right axis of the transform in world space.</returns>
			glm::vec3 GetRight() const;
			
			/// <summary>
			/// Instantaneous face towards target. For a smoothing effect, you can have a vector interpolate towards your target, and face towards the interpolated vector.
			/// </summary>
			/// <param name=""></param>
			void LookAt(const glm::vec3& a_Target, const glm::vec3& a_Up = glm::vec3(0, 1, 0));
			bool operator==(const Th::Components::TransformComponent& rhs);
			bool operator!=(const Th::Components::TransformComponent& rhs);
		private:
			
			entt::entity m_Parent;
			std::vector<entt::entity> m_Children;

			int m_ChildIndex;
			
			glm::mat4 m_WorldMatrix;
			glm::mat4 m_LocalMatrix;

			glm::vec3 m_Position;
			glm::quat m_Rotation;
			glm::vec3 m_Scale;
			glm::vec3 m_EulerAngles{};
		};
	}
}