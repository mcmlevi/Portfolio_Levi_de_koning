#pragma once
#include "Tools/PC/Window/IGUIWindow.h"
#include "Core/FrameListener.h"

namespace ImGui {
}

struct ImGuiStyle;
namespace Frac
{
	enum MenuItemViewFlags
	{
		RENDER_SETTINGS = 1 << 0, // binary 0001
		ENTITY_LIST = 1 << 1, // binary 0010
		COMPONENT_INSPECTOR = 1 << 2, // binary 0100
	};

	class GraphicalUserInterface : public FrameListener
	{
	public:
		GraphicalUserInterface();
		~GraphicalUserInterface();

		void FrameStarted() override;
		void FrameEnded() override {};

		void Update();

		float GetGUIScale() const;

	private:
		void UpdateDocking();

		int m_menuItemViewFlag;

		std::unique_ptr<IGUIWindow> m_renderSettings;
		std::unique_ptr<IGUIWindow> m_entityList;
		std::unique_ptr<IGUIWindow> m_componentInspector;

		ImGuiStyle* m_style;

		void SetupImGuiStyle();

		float m_guiScale;
	};
} // namespace Frac