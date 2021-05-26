#pragma once
#include <WRL.h>
#include <Graphics/D3D12/D3D12CommandList.h>
#include <Resources/D3D12/D3D12UploadBuffer.h>

namespace Th
{
	class D3D12Device;
	class D3D12Resource;
	class D3D12Buffer;
	class D3D12VertexBuffer;
	class D3D12IndexBuffer;

	/// <summary>
	/// Represents a command list from directX12 used to record rendering commands for the renderer to process.
	/// </summary>
	class D3D12CommandList
	{
	public:
		D3D12CommandList();
		~D3D12CommandList() = default;

		void Initialize(D3D12Device& device, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE type);

		D3D12_COMMAND_LIST_TYPE GetCommandListType() const;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> GetGraphicsCommandList() const;

		void TransitionResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);

		void SetVertexBuffer(uint32_t slot, const D3D12VertexBuffer& vertexBuffer);
		void SetIndexBuffer(const D3D12IndexBuffer& indexBuffer);

		void CopyVertexBuffer(D3D12VertexBuffer& vertexBuffer, size_t numVertices, size_t vertexStride, const void* vertexBufferData);
		template<typename T>
		void CopyVertexBuffer(D3D12VertexBuffer& vertexBuffer, const std::vector<T>& vertexBufferData);

		void CopyIndexBuffer(D3D12IndexBuffer& indexBuffer, size_t numIndicies, DXGI_FORMAT indexFormat, const void* indexBufferData);
		template<typename T>
		void CopyIndexBuffer(D3D12IndexBuffer& indexBuffer, const std::vector<T>& indexBufferData);

		void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology);

		void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex, uint32_t startInstance);
		void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance);

		void SetPipelineStateObject(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateObject);

		void Close();
		void Reset(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator);

		void ReleaseTrackedObjects();

	private:
		void CopyBuffer(D3D12Buffer& buffer, size_t numElements, size_t elementSize, const void* bufferData, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE);

		void TrackResource(Microsoft::WRL::ComPtr<ID3D12Object> object);

		std::vector<Microsoft::WRL::ComPtr<ID3D12Object>> m_TrackedObjects;

		D3D12_COMMAND_LIST_TYPE m_CommandListType;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> m_CommandList;

		D3D12Device m_Device;
		D3D12UploadBuffer m_UploadBuffer;
	};

	template<typename T>
	void Th::D3D12CommandList::CopyIndexBuffer(D3D12IndexBuffer& indexBuffer, const std::vector<T>& indexBufferData)
	{
		assert(sizeof(T) == 2 || sizeof(T) == 4); //Assert when using an unsupported format, 16 or 32 bit integers are supported.

		DXGI_FORMAT indexFormat = (sizeof(T) == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
		CopyIndexBuffer(indexBuffer, indexBufferData.size(), indexFormat, indexBufferData.data());
	}

	template<typename T>
	void Th::D3D12CommandList::CopyVertexBuffer(D3D12VertexBuffer& vertexBuffer, const std::vector<T>& vertexBufferData)
	{
		CopyVertexBuffer(vertexBuffer, vertexBufferData.size(), sizeof(T), vertexBufferData.data());
	}

}