#pragma once
#include <d3d12.h>
#include <Graphics/D3D12/D3D12Device.h>

namespace Th
{
	class D3D12CommandList;
	class Win32Window;
	class D3D12CommandQueue;
	class ResizeEventArgs;

	/// <summary>
	/// The D3D12 render context manages the backbuffer swapping on the D3D12 renderer.
	/// </summary>
	class D3D12RenderContext
	{
	public:
		D3D12RenderContext();
		~D3D12RenderContext() = default;

		void Initialize(D3D12Device& device, Win32Window& window, unsigned int backFrameBufferSize = 0);
		void Destroy();

		void SwapBuffers(D3D12CommandList* commandList);

		unsigned int GetCurrentBackBufferIndex() const;
		Microsoft::WRL::ComPtr<ID3D12Resource> GetCurrentBackBuffer() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentTargetView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilTargetView() const;

		D3D12CommandList* ClearColor();

		unsigned int GetClientWidth() const;
		unsigned int GetClientHeight() const;

		const D3D12_VIEWPORT& GetViewport() const;
		const D3D12_RECT& GetScissorRect() const;

	private:
		void CreateRTVDescriptorHeap();
		void CreateDSVDescriptorHeap();
		void UpdateRenderTargetViews();

		void ResizeSwapChain();
		void ResizeDepthBuffer();
		void OnWindowResize(ResizeEventArgs& e);

		D3D12Device m_Device;

		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_BackBuffers;
		std::vector<uint64_t> m_FenceValues;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVDescriptorHeap;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthBuffer;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVHeap;

		D3D12_VIEWPORT m_Viewport;
		D3D12_RECT m_ScissorRect;

		int m_RTVDescriptorSize = 0;
		int m_CurrentBufferIndex = 0;
		int m_BackBufferCount = 0;
		int m_ClientWidth = 0;
		int m_ClientHeight = 0;
	};
}