#pragma once

#include <Graphics/Window/Win32Window.h>
#include <Graphics/D3D12/D3D12Device.h>
#include <Graphics/ImGui/D3D12ImGuiInstance.h>

namespace Th
{
	class D3D12ImGuiInstance;
	class D3D12CommandList;

	/// <summary>
	/// The debug renderer will only be created when running in debug. The current version only supports an ImGUI debug interface.
	/// </summary>
	class D3D12DebugRenderer
	{
	public:
		D3D12DebugRenderer() = default;
		~D3D12DebugRenderer() = default;

		void Initialize(D3D12Device& device, Win32Window& window, unsigned int backBufferCount);
		void Destroy();

		void RecordPreRenderCommands(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList);
		void RecordPostRenderCommands(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList);

	private:
		D3D12Device m_Device;
		D3D12ImGuiInstance m_DebugUIInstance;
	};
}