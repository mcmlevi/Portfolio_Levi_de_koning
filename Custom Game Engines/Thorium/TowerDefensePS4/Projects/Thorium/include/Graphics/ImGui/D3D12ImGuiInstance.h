#pragma once
#include <Graphics/Window/Win32Window.h>
#include <string>
#include <queue>
#include <Events/EventArgs.h>
struct ID3D12GraphicsCommandList;
struct ID3D12DescriptorHeap;

namespace Th
{
	class D3D12Device;
	class IImguiLayer;

	/// <summary>
	/// The Imgui d3d12 instance manages the debug user interface used only in the debug configuration.
	/// </summary>
	class D3D12ImGuiInstance
	{
	public:
		void Initialize(D3D12Device& device, Th::Win32Window& window, unsigned int backBufferCount);
		void ShutDown();
		void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList);
	
	private:
		void InitWindow(Th::Win32Window& window);
		void InitDx12(unsigned int backBufferCount, D3D12Device& device);
		void KeyPressedEvent(Th::KeyEventArgs& args);
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvDescHeap{nullptr};
		std::vector<IImguiLayer*> m_layers{};
	};
}

