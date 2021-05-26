#pragma once
#include <Graphics/ImGui/Layers/IImguiLayer.h>

namespace Th
{
	class DockingLayer : public IImguiLayer
	{
	public:
		DockingLayer() = default;
		virtual ~DockingLayer() = default;
		void Update() override;
	};
}


