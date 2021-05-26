#include "Thpch.h"
#include <Graphics/ImGui/D3D12ImGuiInstance.h>
#include <Graphics/ImGui/imgui_impl_win32.h>
#include "Graphics/ImGui/imgui_impl_dx12.h"
#include <Graphics/ImGui/imgui.h>
#include <Graphics/ImGui/Layers/SceneGraphLayer.h>
#include <Graphics/ImGui/Layers/DockingLayer.h>
#include <Graphics/ImGui/Layers/ConsoleLayer.h>
#include <Graphics/D3D12/D3D12Device.h>
#include <Core/MessageLogger.h>
#include <Events/EventMessenger.h>
#include <Events/KeyCodes.h>
#include <Events/InputHandler.h>
namespace Th
{

	void D3D12ImGuiInstance::Initialize(D3D12Device& device, Th::Win32Window& window, unsigned int backBufferCount)
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		  // Enable Docking
		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		InitWindow(window);
		InitDx12(backBufferCount, device);
		m_layers.push_back(new Th::DockingLayer{});
		m_layers.push_back(new Th::SceneGraphLayer{});
		m_layers.push_back(new Th::ConsoleLayer{});
		// Connect the Imgui instance to the input handle needed to handle inputs for imgui
		Th::InputHandler::getInstance().GetKeyPresedEventDelegate().Connect(&D3D12ImGuiInstance::KeyPressedEvent,this);
		
	}

	void D3D12ImGuiInstance::ShutDown()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void D3D12ImGuiInstance::Render(ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		for (IImguiLayer* layer : m_layers)
		{
			layer->Update();
		}
		ID3D12DescriptorHeap* const ppDescriptorHeaps[] =
		{
			m_srvDescHeap.Get()
		};
		
		commandList->SetDescriptorHeaps(_countof(ppDescriptorHeaps), ppDescriptorHeaps);
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());
	}

	void D3D12ImGuiInstance::InitWindow(Th::Win32Window& window)
	{
		ImGui_ImplWin32_Init(window.GetWindowHandle());
	}

	void D3D12ImGuiInstance::InitDx12(unsigned int backBufferCount, D3D12Device& device)
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		D3D_CHECK(device.Get()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_srvDescHeap)));

		ImGui_ImplDX12_Init(device.Get().Get(),
			backBufferCount, DXGI_FORMAT_R8G8B8A8_UNORM, m_srvDescHeap.Get(), 
			m_srvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			m_srvDescHeap->GetGPUDescriptorHandleForHeapStart());
	}

	void D3D12ImGuiInstance::KeyPressedEvent(Th::KeyEventArgs& args)
	{
		ImGui::GetIO().AddInputCharacter(args.Char);
	}
}
