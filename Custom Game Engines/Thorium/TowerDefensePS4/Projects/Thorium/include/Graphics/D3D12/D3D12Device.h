#pragma once
#include <Utility/D3DErrorHandler.h>

#include <d3d12.h>
#include <Windows.h>
#include <wrl.h>
#include <dxgi1_6.h>

namespace Th
{
	class D3D12CommandQueue;
	class D3D12CommandList;

	/// <summary>
	/// The D3D12 device serves as a wrapper for the D3D12 instance api.
	/// </summary>
	class D3D12Device
	{
	public:
		D3D12Device() = default;
		~D3D12Device() = default;

		/// <summary>
		/// Initialize the D3D12 device.
		/// </summary>
		/// <param name="useWarp">Whether to use the Windows Advanced Rasterize Platform.</param>
		void Initialize(bool useWarp = false);

		/// <summary>
		/// Cleanup of the device and it's dependencies.
		/// </summary>
		void Destroy();

		/// <summary>
		/// Retrieves the actual D3D12 device.
		/// </summary>
		/// <returns>Returns an ID3D12Device2.</returns>
		Microsoft::WRL::ComPtr<ID3D12Device2> Get() const;

		/// <summary>
		/// Checks if WARP is enable or not.
		/// </summary>
		/// <returns>Returns whether warp is enabled or not.</returns>
		bool DoesUseWarp() const;

		/// <summary>
		/// Retrieves a command queue selected by the type the user is in need of.
		/// </summary>
		/// <param name="type">The type of command queue.</param>
		/// <returns>The command queue the user requested, will assert an error when an invalid is given.</returns>
		D3D12CommandQueue* GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) const;


		/// <summary>
		/// Flushes all the command queues.
		/// </summary>
		void Flush();

	private:
		Microsoft::WRL::ComPtr<ID3D12Device2> CreateD3DDevice();

		/// <summary>
		/// Selects the adapter based on whether it's compatible and the memory size.
		/// </summary>
		/// <param name="useWARP">Whether to use the Windows Advanced Rasterize Platform.</param>
		/// <returns>Returns an adapter compatible with DirectX, which supports at least feature level 11. </returns>
		Microsoft::WRL::ComPtr<IDXGIAdapter4> SelectCompatibleAdapter(bool useWARP);

		D3DErrorHandler m_ErrorHandler;
		Microsoft::WRL::ComPtr<ID3D12Device2> m_D3D12Device;
		Microsoft::WRL::ComPtr<IDXGIAdapter4> m_SelectedAdapter;
		bool m_UseWarp = false;

		D3D12CommandQueue* m_DirectCommandQueue = nullptr;
		D3D12CommandQueue* m_ComputeCommandQueue = nullptr;
		D3D12CommandQueue* m_CopyCommandQueue = nullptr;
	};
}