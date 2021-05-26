#include <Thpch.h>
#include <Graphics/D3D12/D3D12CommandQueue.h>
#include <Graphics/D3D12/D3D12CommandList.h>
#include <Utility/D3DErrorHandler.h>

namespace Th
{
	void D3D12CommandQueue::Initialize(D3D12Device* device, D3D12_COMMAND_LIST_TYPE type)
	{
		m_Device = device;
		m_FenceValue = 1;

		D3D12_COMMAND_QUEUE_DESC commandQueueDesc =
		{
			type,
			D3D12_COMMAND_QUEUE_PRIORITY::D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE,
			0
		};

		D3D_CHECK(device->Get()->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_CommandQueue)));
		D3D_CHECK(device->Get()->CreateFence(m_FenceValue, D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));

		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COPY:
			m_CommandQueue->SetName(L"CopyCommandQueue");
			break;
		case D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE:
			m_CommandQueue->SetName(L"ComputeCommandQueue");
			break;
		case D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT:
			m_CommandQueue->SetName(L"DirectCommandQueue");
			break;
		}
	}

	void D3D12CommandQueue::Destroy()
	{
		Flush();
	}

	D3D12CommandList* D3D12CommandQueue::GetCommandList()
	{
		ComPtr<ID3D12CommandAllocator> commandAllocator;
		D3D12CommandList* commandList;

		if (!m_CommandAllocatorQueue.empty() && IsFenceComplete(std::get<0>(m_CommandAllocatorQueue.front())))
		{
			commandAllocator = std::get<1>(m_CommandAllocatorQueue.front());
			m_CommandAllocatorQueue.pop();
			D3D_CHECK(commandAllocator->Reset());
		}
		else
		{
			D3D_CHECK(m_Device->Get()->CreateCommandAllocator(m_CommandListType, IID_PPV_ARGS(&commandAllocator)));
		}

		if (!m_CommandListQueue.empty())
		{
			commandList = m_CommandListQueue.front();
			m_CommandListQueue.pop();
			commandList->Reset(commandAllocator);
		}
		else
		{
			commandList = new D3D12CommandList();
			commandList->Initialize(*m_Device, commandAllocator, m_CommandListType);
		}

		D3D_CHECK(commandList->GetGraphicsCommandList()->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), commandAllocator.Get()));

		return commandList;
	}

	uint64_t D3D12CommandQueue::ExecuteCommandList(D3D12CommandList* commandList)
	{
		commandList->Close();

		ID3D12CommandAllocator* commandAllocator;
		UINT dataSize = sizeof(commandAllocator);
		D3D_CHECK(commandList->GetGraphicsCommandList()->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAllocator));

		ID3D12CommandList* const ppCommandList[] =
		{
			commandList->GetGraphicsCommandList().Get()
		};

		m_CommandQueue->ExecuteCommandLists(1, ppCommandList);
		uint64_t fenceValue = Signal();

		m_CommandAllocatorQueue.emplace(CommandListEntry(fenceValue, commandAllocator));
		m_CommandListQueue.push(commandList);

		commandAllocator->Release();

		return fenceValue;
	}

	uint64_t D3D12CommandQueue::Signal()
	{
		uint64_t fenceValue = ++m_FenceValue;
		m_CommandQueue->Signal(m_Fence.Get(), fenceValue);
		return fenceValue;
	}

	bool D3D12CommandQueue::IsFenceComplete(uint64_t fenceValue)
	{
		return m_Fence->GetCompletedValue() >= fenceValue;
	}

	void D3D12CommandQueue::WaitForFenceValue(uint64_t fenceValue)
	{
		if (!IsFenceComplete(fenceValue))
		{
			auto fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			assert(fenceEvent && "Failed to create fence event handle!");

			m_Fence->SetEventOnCompletion(fenceValue, fenceEvent);
			WaitForSingleObject(fenceEvent, DWORD_MAX);

			CloseHandle(fenceEvent);
		}
	}

	void D3D12CommandQueue::Flush()
	{
		WaitForFenceValue(Signal());
	}

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> D3D12CommandQueue::GetD3D12CommandQueue() const
	{
		return m_CommandQueue;
	}
}