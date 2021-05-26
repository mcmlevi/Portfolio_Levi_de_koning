#include <Thpch.h>
#include <d3d12.h>
#include <Graphics/D3D12/D3D12DebugRenderer.h>
#include <Graphics/D3D12/D3D12CommandList.h>
#include <Events/EventMessenger.h>

#include <Graphics/ImGui/imgui_impl_dx12.h>
#include <Graphics/ImGui/imgui.h>
#include <Graphics/ImGui/imgui_impl_win32.h>

namespace Th
{
	void D3D12DebugRenderer::Initialize(D3D12Device& device, Win32Window& window, unsigned int backBufferCount)
	{
		EventMessenger::GetInstance().ConnectListener<ComPtr<ID3D12GraphicsCommandList2>>("PreDebugRender", &D3D12DebugRenderer::RecordPreRenderCommands, this);
		EventMessenger::GetInstance().ConnectListener<ComPtr<ID3D12GraphicsCommandList2>>("PostDebugRender", &D3D12DebugRenderer::RecordPostRenderCommands, this);

		m_Device = device;
		m_DebugUIInstance.Initialize(device, window, backBufferCount);
	}

	void D3D12DebugRenderer::Destroy()
	{
		EventMessenger::GetInstance().DisconnectListener<ComPtr<ID3D12GraphicsCommandList2>>("PostDebugRender", &D3D12DebugRenderer::RecordPostRenderCommands, this);
		EventMessenger::GetInstance().DisconnectListener<ComPtr<ID3D12GraphicsCommandList2>>("PreDebugRender", &D3D12DebugRenderer::RecordPreRenderCommands, this);

		m_DebugUIInstance.ShutDown();
	}

	void D3D12DebugRenderer::RecordPreRenderCommands(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void D3D12DebugRenderer::RecordPostRenderCommands(ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		m_DebugUIInstance.Render(commandList);
	}
}