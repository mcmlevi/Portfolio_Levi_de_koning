#include "pch.h"
#include "Tools/PC/Window/EntityList.h"

#include <DearImGui\imgui.h>

#include "CoreRenderAPI/Components/Transform.h"

namespace Frac
{
	struct EntityListWindowHandle {}; // Flecs Tag

	EntityList::EntityList(ECSWrapper& a_flecsWorld) : 
		m_flecsWorld(&a_flecsWorld), 
		m_entityCount(0), 
		m_selectedItem(0), 
		m_selectableWidgetXOffset(30.0f),
		m_selectedEntity(0)
	{
		std::function<void(EntityHandle, EntityListWindowHandle&)> imGuiBegin = [this](EntityHandle, EntityListWindowHandle&)
		{
			if (m_windowState)
			{
				ImGui::SetNextWindowPos(ImVec2(ImGui::GetFrameHeight()*1.0f, ImGui::GetFrameHeight()*2.0f), ImGuiCond_Once);
				ImGui::SetNextWindowSize(ImVec2(ImGui::GetFontSize()*20.0f, ImGui::GetFontSize()*40.0f), ImGuiCond_Once);
				ImGui::Begin("Entity List", nullptr, ImGuiWindowFlags_NoCollapse);
				m_entityCount = 0;
			}
		};
		std::function<void(EntityHandle, TOR::Transform&)> addEntityWithTransformComponent = [this](EntityHandle entity, TOR::Transform& transform)
		{
			if (m_windowState)
			{
				if (m_flecsWorld->getName(entity).find("Tile") == std::string::npos) { // Ignore tile entities for now as there are several thousand of them which causes the FPS to drop
					int imguiID = 0;
					m_entityCount++;
					std::string itemid = "##" + std::to_string(m_entityCount);
					ImGui::PushID(imguiID++);
					ImVec2 currentCursorPos = ImGui::GetCursorPos();
					ImGui::SetCursorPos(ImVec2(currentCursorPos.x + m_selectableWidgetXOffset, currentCursorPos.y));
					if (ImGui::Selectable(itemid.c_str(), m_entityCount == m_selectedItem)) {
						m_selectedItem = m_entityCount;
						m_selectedEntity = entity;
					}
					ImGui::SameLine();
					currentCursorPos = ImGui::GetCursorPos();
					ImGui::SetCursorPos(ImVec2(currentCursorPos.x - m_selectableWidgetXOffset, currentCursorPos.y));
					if (ImGui::TreeNode(m_flecsWorld->getName(entity).c_str()))
					{
						ImGui::TreePop();
					}
					ImGui::PopID();
				}
			}
		};
		std::function<void(EntityHandle, EntityListWindowHandle&)> imGuiEnd = [this](EntityHandle, EntityListWindowHandle&)
		{
			if (m_windowState)
			{
				ImGui::End(); // end Entity List Window
			}
		};

		m_flecsWorld->add<EntityListWindowHandle>(m_flecsWorld->makeEntity("EntityViewDummy"));
		m_flecsWorld->makeSystem<EntityListWindowHandle>(imGuiBegin, ePipelineStage::postUpdate, "ImGui Begin");
		//m_flecsWorld->makeSystem<TOR::Transform>(addEntityWithTransformComponent, ePipelineStage::postUpdate, "Get Entity with Transform component System");
		m_flecsWorld->makeSystem<EntityListWindowHandle>(imGuiEnd, ePipelineStage::postUpdate, "ImGui End");
	}
	EntityHandle EntityList::GetSelectedEntity() const
	{
		return m_selectedEntity;
	}
} // namespace Frac