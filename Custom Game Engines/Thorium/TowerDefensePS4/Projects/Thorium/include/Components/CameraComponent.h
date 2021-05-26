#pragma once
#include <glm/glm.hpp>
#include <Events/EventArgs.h>

namespace Th
{
	namespace Components
	{
		struct TransformComponent;

		enum class CameraMode
		{
			Perspective,
			Ortographic
		};

		/// <summary>
		/// The camera represents point in space used to transform transforms into local camera space..
		/// </summary>
		struct CameraComponent
		{

		public:
			CameraComponent();
			~CameraComponent() = default;

			void Initialize(float fov, float width, float height, CameraMode mode = CameraMode::Perspective, 
				float zNear = 0.1f, float zFar = 1000.0f);
			void Destroy();

			/// <summary>
			/// Set the camera mode of this camera, by default it is set to perspective.
			/// </summary>
			/// <param name=""></param>
			void SetCameraMode(const CameraMode&);

			/// <summary>
			/// </summary>
			/// <returns>The camera mode of this camera.</returns>
			CameraMode GetCameraMode() const;

			/// <summary>
			/// Set the field of view of this camera, by default it is set to 70.
			/// </summary>
			/// <param name=""></param>
			void SetFOV(const float&);
			
			/// <summary>
			/// </summary>
			/// <returns>The field of view of this camera.</returns>
			float GetFOV() const;

			/// <summary>
			/// Set the far clipping plane of this camera.
			/// </summary>
			void SetFarClipPlane(const float a_FarClipPlane);

			/// <summary>
			/// </summary>
			/// <returns>The far clipping plane of this camera.</returns>
			float GetFarClipPlane() const;

			/// <summary>
			/// Sets the near clipping plane of this camera.
			/// </summary>
			void SetNearClipPlane(const float a_NearClipPlane);

			/// <summary>
			/// </summary>
			/// <returns>Returns the near clipping plane of this camera.</returns>
			float GetNearClipPlane() const;
			
			/// <summary>
			/// </summary>
			/// <returns>The aspect ratio with the window size specified on this camera, this can be set with "SetWindowSize".</returns>
			float GetAspectRatio() const;

			/// <summary>
			/// Returns the projection matrix of this camera.
			/// </summary>
			/// <returns></returns>
			glm::mat4 GetProjection() const;

		private:
			void UpdateCameraAspects(ResizeEventArgs& e);
			void CalculateProjectionMatrix();

			CameraMode m_Mode;

			float m_FieldOfView;
			float m_AspectRatio;
			float m_Width;
			float m_Height;
			float m_ClipNear;
			float m_ClipFar;

			glm::mat4 m_ProjectionMatrix;
		};
	}
}