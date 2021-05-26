#include <Thpch.h>
#include <Graphics/D3D12/D3D12RenderContext.h>
#include <Utility/D3DErrorHandler.h>
#include <Graphics/D3D12/D3D12Device.h>
#include <Graphics/D3D12/D3D12CommandQueue.h>
#include <Graphics/Window/Win32Window.h>
#include <Events/EventArgs.h>
#include <Events/EventMessenger.h>
#include <Graphics/D3D12/D3D12CommandList.h>

namespace Th
{

	D3D12RenderContext::D3D12RenderContext() : 
		m_ScissorRect(D3D12_RECT(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX))),
		m_Viewport(CD3DX12_VIEWPORT(0.0f, 0.0f, 1.0f, 1.0f)) {}

	void D3D12RenderContext::Initialize(D3D12Device& device, Win32Window& window, unsigned int backBufferCount)
	{
		m_Device = device;
		m_ClientWidth = window.GetWindowWidth();
		m_ClientHeight = window.GetWindowHeight();
		m_BackBufferCount = backBufferCount;
		m_FenceValues.resize(backBufferCount);
		m_BackBuffers.resize(backBufferCount);
		
		m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(m_ClientWidth), static_cast<float>(m_ClientHeight));
		EventMessenger::GetInstance().ConnectListener<ResizeEventArgs&>("OnWindowResizeRequest", &D3D12RenderContext::OnWindowResize, this);

		ComPtr<IDXGIFactory4> dxgiFactory4;
		UINT createFactoryFlags = 0;

#if defined(_DEBUG)
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

		D3D_CHECK(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)));

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = m_ClientWidth;
		swapChainDesc.Height = m_ClientHeight;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.Stereo = FALSE;
		swapChainDesc.SampleDesc = { 1, 0 };
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = backBufferCount;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc.Flags = 0;

		ComPtr<IDXGISwapChain1> swapChain1;
		D3D_CHECK(dxgiFactory4->CreateSwapChainForHwnd(
			m_Device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)->GetD3D12CommandQueue().Get(),
			window.GetWindowHandle(), 
			&swapChainDesc, 
			nullptr, 
			nullptr, 
			&swapChain1));
		D3D_CHECK(dxgiFactory4->MakeWindowAssociation(window.GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER));
		D3D_CHECK(swapChain1.As(&m_SwapChain));

		m_CurrentBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

		CreateRTVDescriptorHeap();

		m_RTVDescriptorSize = m_Device.Get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		
		UpdateRenderTargetViews();
		CreateDSVDescriptorHeap();
		ResizeDepthBuffer();
	}

	void D3D12RenderContext::Destroy()
	{
		EventMessenger::GetInstance().DisconnectListener<ResizeEventArgs&>("OnWindowResizeRequest", &D3D12RenderContext::OnWindowResize, this);
	}

	unsigned int D3D12RenderContext::GetCurrentBackBufferIndex() const
	{
		return m_CurrentBufferIndex;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> D3D12RenderContext::GetCurrentBackBuffer() const
	{
		return m_BackBuffers[m_CurrentBufferIndex];
	}

	D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderContext::GetCurrentTargetView() const
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			m_CurrentBufferIndex,
			m_RTVDescriptorSize);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderContext::GetDepthStencilTargetView() const
	{
		return m_DSVHeap->GetCPUDescriptorHandleForHeapStart();
	}

	D3D12CommandList* D3D12RenderContext::ClearColor()
	{
		auto commandList = m_Device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)->GetCommandList();
		commandList->TransitionResource(m_BackBuffers[m_CurrentBufferIndex],
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET);
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrentBufferIndex, m_RTVDescriptorSize);

		const float clearColor[] = { 0.2f, 0.7f, 0.4f, 1.0f };
		commandList->GetGraphicsCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		commandList->GetGraphicsCommandList()->ClearDepthStencilView(m_DSVHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		commandList->GetGraphicsCommandList()->RSSetViewports(1, &m_Viewport);
		commandList->GetGraphicsCommandList()->RSSetScissorRects(1, &m_ScissorRect);
		commandList->GetGraphicsCommandList()->OMSetRenderTargets(1, &GetCurrentTargetView(), FALSE, &GetDepthStencilTargetView());

		return commandList;
	}

	unsigned int D3D12RenderContext::GetClientWidth() const
	{
		return m_ClientWidth;
	}

	unsigned int D3D12RenderContext::GetClientHeight() const
	{
		return m_ClientHeight;
	}

	const D3D12_VIEWPORT& D3D12RenderContext::GetViewport() const
	{
		return m_Viewport;
	}

	const D3D12_RECT& D3D12RenderContext::GetScissorRect() const
	{
		return m_ScissorRect;
	}

	void D3D12RenderContext::SwapBuffers(D3D12CommandList* commandList)
	{
		D3D12CommandQueue* commandQueue = m_Device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
		
		commandList->TransitionResource(m_BackBuffers[m_CurrentBufferIndex], 
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET, 
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT);

		m_FenceValues[m_CurrentBufferIndex] = commandQueue->ExecuteCommandList(commandList);
		m_SwapChain->Present(1, 0); //TODO: Implement Vsync and tearing.
		m_CurrentBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
		commandQueue->WaitForFenceValue(m_FenceValues[m_CurrentBufferIndex]);
	}

	void D3D12RenderContext::ResizeSwapChain()
	{
		for (int i = 0; i < m_BackBufferCount; ++i)
		{
			m_BackBuffers[i].Reset();
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		
		D3D_CHECK(m_SwapChain->GetDesc(&swapChainDesc));
		D3D_CHECK(m_SwapChain->ResizeBuffers(
			m_BackBufferCount, 
			m_ClientWidth,
			m_ClientHeight,
			swapChainDesc.BufferDesc.Format, 
			swapChainDesc.Flags
		));

		m_CurrentBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

		UpdateRenderTargetViews();
	}

	void D3D12RenderContext::ResizeDepthBuffer()
	{
		D3D12_CLEAR_VALUE optimizedClearValue =
		{
			DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT,
			{ 1.0f, 0 }
		};

		D3D_CHECK(m_Device.Get()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT, m_ClientWidth, m_ClientHeight, 
				1, 0, 1, 0, D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&optimizedClearValue,
			IID_PPV_ARGS(&m_DepthBuffer)
		));

		D3D12_DEPTH_STENCIL_VIEW_DESC dsv =
		{
			DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT,
			D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2D,
			D3D12_DSV_FLAGS::D3D12_DSV_FLAG_NONE
		};
		dsv.Texture2D.MipSlice = 0;

		m_Device.Get()->CreateDepthStencilView(m_DepthBuffer.Get(), &dsv, m_DSVHeap->GetCPUDescriptorHandleForHeapStart());
	}

	void D3D12RenderContext::OnWindowResize(ResizeEventArgs& e)
	{
		if (e.Width != m_ClientWidth || e.Height != m_ClientHeight)
		{
			m_Device.Flush();

			m_ClientWidth = std::max(1, e.Width);
			m_ClientHeight = std::max(1, e.Height);

			m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(e.Width), static_cast<float>(e.Height));

			ResizeSwapChain();
			ResizeDepthBuffer();
		}
	}

	void D3D12RenderContext::CreateRTVDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc =
		{
			D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			m_BackBufferCount,
			D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			0
		};

		D3D_CHECK(m_Device.Get()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RTVDescriptorHeap)));
	}

	void D3D12RenderContext::CreateDSVDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = 
		{
			D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
			1,
			D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			0
		};

		D3D_CHECK(m_Device.Get()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DSVHeap)));
	}

	void D3D12RenderContext::UpdateRenderTargetViews()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		for (int i = 0; i < m_BackBufferCount; ++i)
		{
			ComPtr<ID3D12Resource> backBuffer;
			D3D_CHECK(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));
			m_Device.Get()->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

			m_BackBuffers[i] = backBuffer;
			rtvHandle.Offset(m_RTVDescriptorSize);
		}
	}
}