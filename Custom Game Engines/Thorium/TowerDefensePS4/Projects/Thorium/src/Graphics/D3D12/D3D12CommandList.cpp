#include <Thpch.h>
#include <Graphics/D3D12/D3D12CommandList.h>
#include <Graphics/D3D12/D3D12Device.h>
#include <Resources/D3D12/D3D12Resource.h>
#include <Resources/D3D12/D3D12VertexBuffer.h>
#include <Resources/D3D12/D3D12IndexBuffer.h>
#include <Utility/D3DErrorHandler.h>
#include <Core/MessageLogger.h>

namespace Th
{
	D3D12CommandList::D3D12CommandList() : m_CommandListType(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT) {}

	void D3D12CommandList::Initialize(D3D12Device& device, ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE type)
	{
		m_Device = device;
		m_UploadBuffer.Initialize(device);
		D3D_CHECK(m_Device.Get()->CreateCommandList(0, type, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_CommandList)));
	}

	D3D12_COMMAND_LIST_TYPE D3D12CommandList::GetCommandListType() const
	{
		return m_CommandListType;
	}

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> D3D12CommandList::GetGraphicsCommandList() const
	{
		return m_CommandList;
	}

	void D3D12CommandList::TransitionResource(ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), beforeState, afterState);
		m_CommandList->ResourceBarrier(1, &barrier);
	}

	void D3D12CommandList::SetVertexBuffer(uint32_t slot, const D3D12VertexBuffer& vertexBuffer)
	{
		auto vertexBufferView = vertexBuffer.GetVertexBufferView();
		m_CommandList->IASetVertexBuffers(slot, 1, &vertexBufferView);
		TrackResource(vertexBuffer.Get());
	}

	void D3D12CommandList::SetIndexBuffer(const D3D12IndexBuffer& indexBuffer)
	{
		auto indexBufferView = indexBuffer.GetIndexBufferView();
		m_CommandList->IASetIndexBuffer(&indexBufferView);
		TrackResource(indexBuffer.Get());
	}

	void D3D12CommandList::CopyVertexBuffer(D3D12VertexBuffer& vertexBuffer, size_t numVertices, size_t vertexStride, const void* vertexBufferData)
	{
		CopyBuffer(vertexBuffer, numVertices, vertexStride, vertexBufferData);
	}

	void D3D12CommandList::CopyIndexBuffer(D3D12IndexBuffer& indexBuffer, size_t numIndicies, DXGI_FORMAT indexFormat, const void* indexBufferData)
	{
		size_t indexSizeInBytes = indexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4;
		CopyBuffer(indexBuffer, numIndicies, indexSizeInBytes, indexBufferData);
	}

	void D3D12CommandList::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology)
	{
		m_CommandList->IASetPrimitiveTopology(topology);
	}

	void D3D12CommandList::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex, uint32_t startInstance)
	{
		m_CommandList->DrawIndexedInstanced(indexCount, instanceCount, startIndex, baseVertex, startInstance);
	}

	void D3D12CommandList::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance)
	{
		m_CommandList->DrawInstanced(vertexCount, instanceCount, startVertex, startInstance);
	}

	void D3D12CommandList::SetPipelineStateObject(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateObject)
	{
		m_CommandList->SetPipelineState(pipelineStateObject.Get());
		TrackResource(pipelineStateObject);
	}

	void D3D12CommandList::Close()
	{
		D3D_CHECK(m_CommandList->Close());
	}

	void D3D12CommandList::Reset(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator)
	{
		m_CommandList->Reset(allocator.Get(), nullptr);
		m_UploadBuffer.Reset();

		ReleaseTrackedObjects();
	}

	void D3D12CommandList::ReleaseTrackedObjects()
	{	
		m_TrackedObjects.clear();
	}

	void D3D12CommandList::CopyBuffer(D3D12Buffer& buffer, size_t numElements, size_t elementSize, const void* bufferData, D3D12_RESOURCE_FLAGS flags /*= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE*/)
	{
		size_t bufferSize = numElements * elementSize;
		ComPtr<ID3D12Resource> resource;

		if (bufferSize > 0)
		{
			// Create a committed resource for the GPU resource in a default heap.
			D3D_CHECK(m_Device.Get()->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags),
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr,
				IID_PPV_ARGS(&resource)));

			//TODO: Track global state.

			// Create a committed resource for the upload.
			if (bufferData != nullptr)
			{
				Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource;
				D3D_CHECK(m_Device.Get()->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
					D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
					D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&uploadResource)));

				D3D12_SUBRESOURCE_DATA subresourceData = {};
				subresourceData.pData = bufferData;
				subresourceData.RowPitch = bufferSize;
				subresourceData.SlicePitch = subresourceData.RowPitch;

				//TODO: Insert transition barrier.

				UpdateSubresources(m_CommandList.Get(), resource.Get(), uploadResource.Get(), 0, 0, 1, &subresourceData);
				TrackResource(uploadResource.Get());
			}
			TrackResource(resource.Get());
		}
		else
		{
			LOGWARNING("Trying to submit a buffer to the GPU that contains 0 elements. Buffer copying has been canceled!");
		}

		buffer.Set(resource);
		buffer.CreateViews(numElements, elementSize); //Calls the inherited class instances methods.
	}

	void D3D12CommandList::TrackResource(Microsoft::WRL::ComPtr<ID3D12Object> object)
	{
		m_TrackedObjects.push_back(object);
	}
}