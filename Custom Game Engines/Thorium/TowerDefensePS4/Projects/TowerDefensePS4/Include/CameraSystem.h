#pragma once
#include "Core\Systems.h"

namespace Th {
	class MouseMotionEventArgs;
	class MouseWheelEventArgs;
	class MouseButtonEventArgs;
	struct KeyEventArgs;
}

class CameraSystem : public Th::Systems
{
public:
	CameraSystem();
	~CameraSystem() = default;

	void OnUpdate(float) override;

	void GetActiveCam(entt::entity&);

	void KeyPressedEvent(Th::KeyEventArgs& args);
	void KeyReleasedEvent(Th::KeyEventArgs& args);
	void MoveForward(float dt);
	void MoveRight(float dt);
	void MoveUP(float dt);
	void Zoom(float dt);
	void RotateMode(float dt);
	void MouseMotion(Th::MouseMotionEventArgs& args);
	void MouseWheel(Th::MouseWheelEventArgs& args);

	void OnMouseDown(Th::MouseButtonEventArgs& args);
	void OnMouseUp(Th::MouseButtonEventArgs& args);
private:
	entt::entity m_Camera;
	float m_Speed;
	Th::TransformComponent m_Root;

	bool forward;
	bool right;
	bool up;

	float x = 0;
	float y = 0;
	float z = 0;

	float m_ScrWheelDelta;
	float m_ScrWheelSpeed;

	bool m_RMB;

	glm::vec3 m_RelativeMouseVal;
	float m_RotationSpeed;

	float m_Length;

	float m_MinXRot;
	float m_MaxXRot;
	float m_MinScroll;
	float m_MaxScroll;

	bool m_FreeFlyCam;
};

