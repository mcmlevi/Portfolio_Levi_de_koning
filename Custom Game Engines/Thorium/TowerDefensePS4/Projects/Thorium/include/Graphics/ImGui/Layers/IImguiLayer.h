#pragma once
namespace Th
{
	class IImguiLayer
	{
	public:
		IImguiLayer() = default;
		virtual ~IImguiLayer() = default;
		virtual void Update() = 0;
	};
}
