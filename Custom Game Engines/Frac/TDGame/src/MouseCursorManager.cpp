#include "pch.h"
#include "MouseCursorManager.h"
#include "Core/EngineCore.h"
#include "Core/ECSWrapper.h"
#include "Components.h"
#include "CoreRenderAPI/Components/Transform.h"

MouseCursorManager::MouseCursorManager(Frac::ECSWrapper& a_world) :
	m_world(a_world)
	, m_theCursorEnt(0)
{
	m_theCursorEnt = m_world.makeEntity<TOR::Transform, Frac::TextComponent>("3DGameplayCursor", 
		{
			glm::vec3(0.f)
			, m_cursorSize}, 
		{
			m_theIcon
			, m_cursorFontSize
			, m_defaultColor
			, Frac::TextComponent::FontFamily::Icons
		});

	Hide(); // start hidden
}

void MouseCursorManager::Update3DMouseCursor(glm::vec3& worldPos)
{
	auto transform = m_world.get<TOR::Transform>(m_theCursorEnt);
	
	transform->Position = worldPos + m_offset;
	
	transform->EulerOrientation.x = glm::radians<float>(90.f);
	transform->EulerOrientation.y += 0.5f * m_world.getDeltaTime();
	
	transform->Orientation = glm::quat(glm::vec3(transform->EulerOrientation));

	if (m_isSelected)
	{
		m_timer += m_world.getDeltaTime();
		transform->Scale = m_cursorSize * ((glm::sin(m_timer) * 0.5f) + 1.5f);
	} else
	{
		transform->Scale = m_cursorSize;
	}
}

void MouseCursorManager::SetColor(const glm::vec3& newColor) const
{
	auto text = m_world.get<Frac::TextComponent>(m_theCursorEnt);
	text->color = newColor;
}

void MouseCursorManager::ResetColor() const
{
	auto text = m_world.get<Frac::TextComponent>(m_theCursorEnt);
	text->color = m_defaultColor;
}

void MouseCursorManager::SetSelectState()
{
	SetColor(m_cursorSelectCol);
	m_isSelected = true;
}

void MouseCursorManager::SetIdleState()
{
	SetColor(m_cursorIdleCol);
	m_isSelected = false;
}

void MouseCursorManager::Show()
{
	auto transform = m_world.get<TOR::Transform>(m_theCursorEnt);
	transform->Scale = glm::vec3(m_cursorSize);
	
	m_isHidden = false;
}

void MouseCursorManager::Hide()
{
	auto transform = m_world.get<TOR::Transform>(m_theCursorEnt);
	transform->Scale = glm::vec3(0.f);
	
	m_isHidden = true;
}

bool MouseCursorManager::IsHidden() const
{
	return m_isHidden;
}
