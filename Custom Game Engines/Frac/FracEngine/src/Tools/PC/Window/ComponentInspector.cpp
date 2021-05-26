#include "pch.h"
#include "Tools/PC/Window/ComponentInspector.h"

#include "Core/ECSWrapper.h"
#include "Tools/PC/Window/EntityList.h"

/// Components
#include "CoreRenderAPI/Components/Transform.h"

#include <DearImGui\imgui.h>

namespace Frac
{
	ComponentInspector::ComponentInspector(ECSWrapper& a_flecsWorld, EntityList& a_entityList) :
		m_flecsWorld(a_flecsWorld),
		m_entityList(a_entityList)
	{

	}

	void ComponentInspector::Update() 
	{
		if (m_windowState) 
		{
			// TODO Reflection to read out known and unknown components automatically
			int imguiID = 0;
			ImGui::SetNextWindowPos(ImVec2(ImGui::GetFrameHeight() * 17.5f, ImGui::GetFrameHeight() * 2.0f), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(ImGui::GetFontSize() * 15.0f, ImGui::GetFontSize() * 20.0f), ImGuiCond_Once);
			ImGui::Begin("Component Inspector", nullptr, ImGuiWindowFlags_NoCollapse);
			if (m_entityList.GetSelectedEntity()) 
			{
				std::string name = "Entity: " + m_flecsWorld.getName(m_entityList.GetSelectedEntity());
				ImGui::Text(name.c_str());
				// Transform Component
				if (m_flecsWorld.has<TOR::Transform>(m_entityList.GetSelectedEntity())) 
				{
					const TOR::Transform& transformComponent = *m_flecsWorld.get<TOR::Transform>(m_entityList.GetSelectedEntity());
					ImGui::Separator();
					ImGui::Text("Transform Component");
					ImGui::Spacing();

					//Position
					ImGui::Spacing();
					ImGui::Text("Position");
					float position[3]{ transformComponent.Position.x, transformComponent.Position.y, transformComponent.Position.z };
					ImGui::PushID(imguiID++);
					ImGui::DragFloat3("", position);
					TOR::Transform newTransformComponent = transformComponent;
					newTransformComponent.Position.x = position[0];
					newTransformComponent.Position.y = position[1];
					newTransformComponent.Position.z = position[2];
					ImGui::PopID();
					
					//Scale
					ImGui::Spacing();
					ImGui::Text("Scale");
					float scale[3]{ transformComponent.Scale.x, transformComponent.Scale.y, transformComponent.Scale.z };
					ImGui::PushID(imguiID++);
					ImGui::DragFloat3("", scale);
					newTransformComponent.Scale.x = scale[0];
					newTransformComponent.Scale.y = scale[1];
					newTransformComponent.Scale.z = scale[2];
					ImGui::PopID();

					//Rotation
					ImGui::Spacing();
					ImGui::Text("Rotation");
					float rotation[3]{ transformComponent.EulerOrientation.x, transformComponent.EulerOrientation.y, transformComponent.EulerOrientation.z };
					ImGui::PushID(imguiID++);
					ImGui::DragFloat3("", rotation, 0.1f);
					newTransformComponent.EulerOrientation.x = rotation[0];
					newTransformComponent.EulerOrientation.y = rotation[1];
					newTransformComponent.EulerOrientation.z = rotation[2];
					newTransformComponent.Orientation = glm::quat(glm::vec3(rotation[0], rotation[1], rotation[2]));
					ImGui::PopID();

					m_flecsWorld.set(m_entityList.GetSelectedEntity(), newTransformComponent);
				} // end Transform Component
			}
			else {
				std::string name = "Entity: No entity selected";
				ImGui::Text(name.c_str());
			}
			ImGui::End(); // end Component Inspector Window
		}
	}
} // namespace Frac