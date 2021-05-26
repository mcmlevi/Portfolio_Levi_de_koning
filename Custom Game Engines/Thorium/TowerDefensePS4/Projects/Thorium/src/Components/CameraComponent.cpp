#include <Thpch.h>
#include <Components/CameraComponent.h>
#include <glm/gtx/compatibility.hpp>
#include <Core/MessageLogger.h>
#include <Events/EventMessenger.h>
#include <Components/TransformComponent.h>

namespace Th
{
	namespace Components
	{
		CameraComponent::CameraComponent() : 
			m_Mode(CameraMode::Perspective),
			m_FieldOfView(70.0f),
			m_Width(0),
			m_Height(0),
			m_ClipNear(0.1f),
			m_ClipFar(1000.0f),
			m_ProjectionMatrix(1.0f) {}

		void CameraComponent::Initialize(float fov, float width, float height, CameraMode mode /*=CameraMode::Perspective*/, float zNear /*= 0.1f*/, float zFar /*= 1000.0f*/)
		{
			m_Mode = mode;
			m_FieldOfView = fov;
			m_Width = width;
			m_Height = height;
			m_AspectRatio = m_Width / m_Height;
			m_ClipNear = zNear;
			m_ClipFar = zFar;

			CalculateProjectionMatrix();
#if PLATFORM_WIN64
			EventMessenger::GetInstance().ConnectListener<ResizeEventArgs&>("OnWindowResizeRequest", &CameraComponent::UpdateCameraAspects, this);
#endif
		}

		void CameraComponent::Destroy()
		{
#if PLATFORM_WIN64
			EventMessenger::GetInstance().DisconnectListener<ResizeEventArgs&>("OnWindowResizeRequest", &CameraComponent::UpdateCameraAspects, this);
#endif
		}

		float CameraComponent::GetAspectRatio() const
		{
			return m_AspectRatio;
		}

		void CameraComponent::SetCameraMode(const CameraMode& a_Mode = CameraMode::Perspective)
		{
			m_Mode = a_Mode;
			CalculateProjectionMatrix();
		}

		CameraMode CameraComponent::GetCameraMode() const
		{
			return m_Mode;
		}

		void CameraComponent::SetFOV(const float& a_FOV = 70.0f)
		{
			m_FieldOfView = a_FOV;
			CalculateProjectionMatrix();
		}

		float CameraComponent::GetFOV() const
		{
			return m_FieldOfView;
		}

		void CameraComponent::SetFarClipPlane(const float a_FarClipPlane)
		{
			m_ClipFar = a_FarClipPlane;
		}

		float CameraComponent::GetFarClipPlane() const
		{
			return m_ClipFar;
		}

		void CameraComponent::SetNearClipPlane(const float a_NearClipPlane)
		{
			m_ClipNear = a_NearClipPlane;
		}

		float CameraComponent::GetNearClipPlane() const
		{
			return m_ClipNear;
		}

		glm::mat4 CameraComponent::GetProjection() const
		{
			return m_ProjectionMatrix;
		}

		void CameraComponent::UpdateCameraAspects(ResizeEventArgs& e)
		{
			m_Width = e.Width;
			m_Height = e.Height;
			m_AspectRatio = m_Width / m_Height;
			CalculateProjectionMatrix();
		}

		void CameraComponent::CalculateProjectionMatrix()
		{
			switch (m_Mode)
			{
			case CameraMode::Perspective:
				m_ProjectionMatrix = glm::perspectiveLH(glm::radians(m_FieldOfView / 2), m_AspectRatio, m_ClipNear, m_ClipFar);
				break;
			case CameraMode::Ortographic:
				m_ProjectionMatrix = glm::ortho(0.0f, m_Width, m_Height, 0.0f, m_ClipNear, m_ClipFar);
				break;
			default:
				LOGERROR("Unspecified camera mode given!");
				break;
			}
		}
	}
}