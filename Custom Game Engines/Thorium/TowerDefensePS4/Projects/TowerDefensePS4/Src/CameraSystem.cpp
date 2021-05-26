#include "pch.h"
#include "CameraSystem.h"


#include "Ray.h"
#include "Components/CameraComponent.h"
#include "Core/MessageLogger.h"
#include "Events/EventMessenger.h"
#include "Events/InputHandler.h"
#include "Events/EventArgs.h"
#include "Core/Audio/AudioComponent.h"

CameraSystem::CameraSystem() : Systems(), forward(false), m_ScrWheelDelta(0), m_RMB(false), m_RelativeMouseVal(0), m_FreeFlyCam(false)
{
	Register();
	Th::EventMessenger::GetInstance().ConnectListener<entt::entity&>("ActiveCameraSet", &CameraSystem::GetActiveCam, this);
	Th::InputHandler::getInstance().GetKeyPresedEventDelegate().Connect(&CameraSystem::KeyPressedEvent, this);
	Th::InputHandler::getInstance().GetKeyReleasedEventDelegate().Connect(&CameraSystem::KeyReleasedEvent, this);
	Th::InputHandler::getInstance().GetMouseMotionDelegate().Connect(&CameraSystem::MouseMotion, this);
	Th::InputHandler::getInstance().GetMouseWheelDelegate().Connect(&CameraSystem::MouseWheel, this);

	Th::InputHandler::getInstance().GetMouseButtonPressedDelegate().Connect(&CameraSystem::OnMouseDown, this);
	Th::InputHandler::getInstance().GetMouseButtonReleasedDelegate().Connect(&CameraSystem::OnMouseUp, this);

	//m_Root.LookAt(glm::vec3(0));
	m_Speed = 20;
	m_ScrWheelSpeed = 10;
	m_RotationSpeed = 0.5f;
	
	m_MinScroll = 2;
	m_MaxScroll = 100;
	m_MinXRot = 2;
	m_MaxXRot = 50;
	//m_Root.SetPosition(glm::vec3(-20, 0 ,-20));
}

void CameraSystem::OnUpdate(float dt)
{
	auto& transform = GetEntityRegister().get<Th::TransformComponent>(m_Camera);
	transform.SetRotation(m_Root.GetRotation());
	Ray ray(m_Root.GetPosition(), m_Root.GetPosition() - m_Root.GetForward() * m_Length);


	transform.SetPosition(ray.m_EndPoint);
	if (m_RMB)
	{
		RotateMode(dt);
	}

	if (forward && !m_RMB)
	{
		MoveForward(dt);
	}
	if (right && !m_RMB)
	{
		MoveRight(dt);
	}
	if (up && !m_RMB) {
		MoveUP(dt);
	}

	Zoom(dt);


	m_RelativeMouseVal = glm::vec3(0);
	m_ScrWheelDelta = 0.0f;
}

void CameraSystem::GetActiveCam(entt::entity& entity)
{
	m_Camera = entity;
	auto& transform = GetEntityRegister().get<Th::Components::TransformComponent>(m_Camera);
	glm::vec3 pos = transform.GetPosition();

	GetEntityRegister().emplace<Th::AudioListener>(m_Camera);


	Th::TransformComponent Root = Th::TransformComponent(glm::vec3(pos.x,3,pos.z),entt::null,glm::vec3(glm::radians(45.0f),0,0));
	Root.SetPosition(glm::vec3(pos.x, 0, pos.z));


	m_Length = glm::length(pos - Root.GetPosition());
	m_Root = Root;
	//m_Root.SetRotation(Root.GetRotation());
	//const glm::vec3 rootTransform = m_Root.GetPosition();
	//transform.SetPosition(glm::vec3(rootTransform.x, transform.GetPosition().y, rootTransform.z));
}

void CameraSystem::KeyPressedEvent(Th::KeyEventArgs& args)
{
	auto temp = args.State;
	switch (args.Key)
	{
	case Th::KeyCode::W:
		y = 1;
		forward = true;
		break;

	case Th::KeyCode::A:
		right = true;
		x = -1;
		break;

	case Th::KeyCode::S:
		forward = true;
		y = -1;

		break;

	case Th::KeyCode::D:
		right = true;
		x = 1;
		break;

	case Th::KeyCode::Q:
		up = true;
		z = -1;
		break;

	case Th::KeyCode::E:
		up = true;
		z = 1;
		break;
	}


}

void CameraSystem::KeyReleasedEvent(Th::KeyEventArgs& args)
{
	auto temp = args.State;
	switch (args.Key)
	{
	case Th::KeyCode::W:
		y = 0;
		forward = false;
		break;

	case Th::KeyCode::A:
		x = 0;
		right = false;
		break;

	case Th::KeyCode::S:
		forward = false;
		y = 0;
		break;

	case Th::KeyCode::D:
		x = 0;
		right = false;
		break;

	case Th::KeyCode::Q:
		up = false;
		z = 0;
		break;

	case Th::KeyCode::E:
		up = false;
		z = 0;
		break;
	}

}

void CameraSystem::MoveForward(float dt)
{
	glm::vec3 pos = m_Root.GetPosition();
	glm::vec3 forwardv = m_Root.GetForward();
	m_Root.SetPosition(pos + glm::vec3(forwardv.x, 0, forwardv.z) * y * m_Speed * dt);
}

void CameraSystem::MoveRight(float dt)
{
	auto& cameracomp = GetEntityRegister().get<Th::Components::CameraComponent>(m_Camera);
	auto& transform = GetEntityRegister().get<Th::Components::TransformComponent>(m_Camera);
	glm::vec3 pos = m_Root.GetPosition();
	const glm::vec3 rightv = m_Root.GetRight();

	m_Root.SetPosition(pos + glm::vec3(rightv.x, 0, rightv.z) * x * m_Speed * dt);
}

void CameraSystem::MoveUP(float dt)
{
	auto& cameracomp = GetEntityRegister().get<Th::Components::CameraComponent>(m_Camera);
	auto& transform = GetEntityRegister().get<Th::Components::TransformComponent>(m_Camera);
	glm::vec3 pos = m_Root.GetPosition();
	const glm::vec3 upv = m_Root.GetUp();

	m_Root.SetPosition(pos + glm::vec3(0, upv.y, 0) * z * m_Speed * dt);
}

void CameraSystem::Zoom(float dt)
{
	m_Length -= m_ScrWheelDelta;

	m_Length = glm::clamp(m_Length, m_MinScroll, m_MaxScroll);
}

void CameraSystem::RotateMode(float dt)
{
	//auto& transform = GetEntityRegister().get<Th::Components::TransformComponent>(m_Camera);

	const glm::vec3 currentRot = m_Root.GetEulerAngles();
	glm::vec3 clampedRot(0);

	clampedRot.x = glm::clamp(currentRot.x + m_RelativeMouseVal.x * dt * m_RotationSpeed, glm::radians(m_MinXRot), glm::radians(m_MaxXRot));
	clampedRot.y = currentRot.y + m_RelativeMouseVal.y * dt * m_RotationSpeed;

	m_Root.SetRotation(clampedRot);
}

void CameraSystem::MouseMotion(Th::MouseMotionEventArgs& args)
{
	m_RelativeMouseVal = glm::vec3(args.RelY, args.RelX, 0);
}

void CameraSystem::MouseWheel(Th::MouseWheelEventArgs& args)
{
	m_ScrWheelDelta = args.WheelDelta;
}

void CameraSystem::OnMouseDown(Th::MouseButtonEventArgs& args)
{
	switch (args.Button)
	{
	case Th::MouseButtonEventArgs::MouseButton::None:
		break;
	case Th::MouseButtonEventArgs::MouseButton::Left:
		break;
	case Th::MouseButtonEventArgs::MouseButton::Right:
		m_RMB = true;
		break;
	case Th::MouseButtonEventArgs::MouseButton::Middel:

		break;
	default:;
	}
}

void CameraSystem::OnMouseUp(Th::MouseButtonEventArgs& args)
{
	switch (args.Button)
	{
	case Th::MouseButtonEventArgs::MouseButton::None:
		break;
	case Th::MouseButtonEventArgs::MouseButton::Left:
		break;
	case Th::MouseButtonEventArgs::MouseButton::Right:
		m_RMB = false;
		break;
	case Th::MouseButtonEventArgs::MouseButton::Middel:
		break;
	default:;
	}
}
