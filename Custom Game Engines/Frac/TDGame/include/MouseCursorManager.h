#pragma once
#include "Core/ECSWrapper.h"

namespace Frac {
	class ECSWrapper;
}

class MouseCursorManager 
{
public:
	MouseCursorManager(Frac::ECSWrapper& a_world);
	~MouseCursorManager() = default;

	void Update3DMouseCursor(glm::vec3& worldPos);

	void SetColor(const glm::vec3&) const;
	void ResetColor() const;

	void SetSelectState();
	void SetIdleState();
	
	void Show();
	void Hide();

	bool IsHidden() const;
	
private:
	Frac::ECSWrapper& m_world;
	Frac::EntityHandle m_theCursorEnt;

	bool m_isHidden		= false;
	bool m_isSelected	= false;
	float m_timer		= 0.f;
	
protected:
	const glm::vec3 m_offset = glm::vec3(0.5f, 0.f, -0.4f); // TODO needs to be tweaked or something
	
	const glm::vec3 m_defaultColor	= glm::vec3(1.f);
	
	const glm::vec3 m_cursorSize	= glm::vec3(0.04f);
	const float m_cursorFontSize	= 0.5f;

	const std::string m_theIcon		= "m";

	const glm::vec3 m_cursorSelectCol	= glm::vec3(0.5f, 0.7f, 1.f);
	const glm::vec3 m_cursorIdleCol		= glm::vec3(1.f, 1.f, 1.f);
};
