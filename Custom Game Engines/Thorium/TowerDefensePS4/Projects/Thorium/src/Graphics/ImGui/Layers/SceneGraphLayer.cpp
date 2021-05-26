#include "Thpch.h"
#include <Graphics/ImGui/Layers/SceneGraphLayer.h>
#include <Core/WorldManager.h>
#include <Graphics/ImGui/imgui.h>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Components/TransformComponent.h>
#include <Components/NameComponent.h>
#include <Components/CameraComponent.h>
#include <Components/ModelComponent.h>
#include <Events/EventMessenger.h>

#include "Core/MessageLogger.h"
#include <glm/gtx/euler_angles.hpp>
#define USE_IMGUI_API
#include <Graphics/ImGui/ImGuizmo.h>



namespace Th
{
	SceneGraphLayer::SceneGraphLayer()
	{
		Th::EventMessenger::GetInstance().ConnectListener<entt::entity&>("ActiveCameraSet", &SceneGraphLayer::CameraSet, this);
	}

	SceneGraphLayer::~SceneGraphLayer()
	{
	}

	void SceneGraphLayer::Update()
	{
		ImGuizmo::BeginFrame();
		ImGuizmo::Enable(true);
		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
		
		if(ImGui::Begin("Scene Graph Tool", &m_active, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar))
		{
			entt::entity worldRoot = Th::WorldManager::GetSingleton().GetWorldRoot();
			AddChild(worldRoot);
		}
		ImGui::End();
	}

	void SceneGraphLayer::AddPosData(Th::Components::TransformComponent& transform)
	{
		glm::vec3 pos = transform.GetPosition();
		glm::vec3 scale = transform.GetScale();
		glm::vec3 rot = glm::degrees(transform.GetEulerAngles());
		if (ImGui::Button("Edit"))
		{
			// Remove 
			if (m_GizmoState == &transform)
			{
				m_GizmoState = nullptr;
			}
			else			
				m_GizmoState = &transform;
		}
		if (m_GizmoState == &transform)
			AddGizmoWidget(transform);
		
		if(ImGui::DragFloat3("Position", glm::value_ptr(pos)))
		{
			transform.SetPosition(pos);
		}
		if(ImGui::DragFloat3("Scale", glm::value_ptr(scale)))
		{
			transform.SetScale(scale);
		}
		if(ImGui::DragFloat3("Rotation", glm::value_ptr(rot)))
		{
			transform.SetRotation(glm::radians(rot));
		}
	}


	void SceneGraphLayer::AddChild(entt::entity child)
	{
		Th::NameComponent& name = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::NameComponent>(child);
		Th::TransformComponent& transform = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::TransformComponent>(child);
		if(ImGui::TreeNode(name.Name.c_str()))
		{
			AddPosData(transform);
			for (int i = 0; i < transform.GetChildCount(); ++i)
			{
				AddChild(transform.GetChild(i));
			}
			ImGui::TreePop();
		}
	}

	void SceneGraphLayer::AddGizmoWidget(Th::Components::TransformComponent& transform)
	{
		static ImGuizmo::OPERATION CurrentGizmoOperation(ImGuizmo::TRANSLATE);
		static ImGuizmo::MODE CurrentGizmoMode(ImGuizmo::WORLD);
		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		if (ImGui::RadioButton("Translate", CurrentGizmoOperation == ImGuizmo::TRANSLATE))
			CurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", CurrentGizmoOperation == ImGuizmo::ROTATE))
			CurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", CurrentGizmoOperation == ImGuizmo::SCALE))
			CurrentGizmoOperation = ImGuizmo::SCALE;


		if (ImGui::RadioButton("World", CurrentGizmoMode == ImGuizmo::WORLD))
			CurrentGizmoMode = ImGuizmo::WORLD;
		ImGui::SameLine();
		if (ImGui::RadioButton("Local", CurrentGizmoMode == ImGuizmo::LOCAL))
			CurrentGizmoMode = ImGuizmo::LOCAL;


		Th::Components::CameraComponent& cam = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::CameraComponent>(m_camera);
		Th::Components::TransformComponent& camT = Th::WorldManager::GetSingleton().GetEntityRegister().get<Th::Components::TransformComponent>(m_camera);
		glm::mat4 projMat = cam.GetProjection();
		glm::mat4 viewMat = glm::inverse(camT.GetWorldMatrix());
		glm::mat4 obj;
		if (CurrentGizmoMode == ImGuizmo::LOCAL)
			obj = transform.GetLocalMatrix();
		else
			obj = transform.GetWorldMatrix();
		float snap{ 0 };
		if(ImGuizmo::Manipulate(glm::value_ptr(viewMat), glm::value_ptr(projMat), CurrentGizmoOperation, CurrentGizmoMode, glm::value_ptr(obj), NULL,nullptr))
		{
			glm::vec3 pos;
			glm::vec3 rot;
			glm::vec3 scale;
			ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(obj), glm::value_ptr(pos), glm::value_ptr(rot), glm::value_ptr(scale));
			if(CurrentGizmoOperation == ImGuizmo::ROTATE)
			{
				transform.SetRotation(glm::radians(rot));
			}
			else if(CurrentGizmoOperation == ImGuizmo::TRANSLATE)
			{
				transform.SetPosition(pos);
			}
			else if (CurrentGizmoOperation == ImGuizmo::SCALE)
			{
				transform.SetScale(scale);
			}
		}
	}


	void SceneGraphLayer::CameraSet(entt::entity& cam)
	{
		m_camera = cam;
	}
}

