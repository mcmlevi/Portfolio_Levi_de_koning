#pragma once
#include <queue>
#include <Graphics/D3D12/D3D12Device.h>
#include <Graphics/D3D12/D3D12CommandList.h>

namespace Th
{
	class D3D12Device;

	/// <summary>
	/// The D3D12CommandQueue handles all the draw events on the GPU side of the render system.
	/// </summary>
	class D3D12CommandQueue
	{
	public:
		D3D12CommandQueue() = default;
		virtual ~D3D12CommandQueue() = default;

		/// <summary>
		/// Initializes the D3D12CommandQueue.
		/// </summary>
		/// <param name="type">The type the Queue should use on the GPU.</param>
		void Initialize(D3D12Device* device, D3D12_COMMAND_LIST_TYPE type);

		/// <summary>
		/// Destroys the D3D12CommandQueue.
		/// </summary>
		void Destroy();

		/// <summary>
		/// Gets an available command list.
		/// </summary>
		/// <returns></returns>
		D3D12CommandList* GetCommandList();

		/// <summary>
		/// Executes a given command list on the Command queue.
		/// </summary>
		/// <param name="commandList">The command list to be executed.</param>
		/// <returns></returns>
		uint64_t ExecuteCommandList(D3D12CommandList* commandList);

		/// <summary>
		/// Increments the fence value and signals the command queue that the fence value has updated.
		/// </summary>
		/// <returns>Returns the new fence value.</returns>
		uint64_t Signal();

		/// <summary>
		/// Checks whether the fence has been completed.
		/// </summary>
		/// <param name="fenceValue">Takes in a certain fence value.</param>
		/// <returns>Returns whether it actually did complete.</returns>
		bool IsFenceComplete(uint64_t fenceValue);

		/// <summary>
		/// Makes the current thread wait until the resource are free on the GPU.
		/// </summary>
		/// <param name="fenceValue">Takes in a certain fence value.</param>
		void WaitForFenceValue(uint64_t fenceValue);

		/// <summary>
		/// Ensures that any commands previously executed on the GPU have 
		/// finished executing before the CPU thread is allowed to continue processing.
		/// </summary>
		void Flush();

		/// <summary>
		/// Retrieves the actual queue this instance wraps around.
		/// </summary>
		/// <returns>Returns the ID3D12CommandQueue.</returns>
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetD3D12CommandQueue() const;

	private:
		using CommandListEntry = std::tuple<uint64_t, ComPtr<ID3D12CommandAllocator>>;

		D3D12_COMMAND_LIST_TYPE                     m_CommandListType;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>  m_CommandQueue;
		Microsoft::WRL::ComPtr<ID3D12Fence>         m_Fence;
		uint64_t                                    m_FenceValue;
		D3D12Device*								m_Device;

		std::queue<CommandListEntry>				m_CommandAllocatorQueue;
		std::queue<D3D12CommandList*>				m_CommandListQueue;
	};
}