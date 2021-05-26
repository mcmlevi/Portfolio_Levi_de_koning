#include <Thpch.h>
#include <Resources/D3D12/D3D12VertexBuffer.h>

namespace Th
{

	D3D12VertexBuffer::D3D12VertexBuffer(const std::wstring& name /*= L""*/)
		: D3D12Buffer(name),
		m_VertexCount(0),
		m_VertexStride(0),
		m_VertexBufferView({}) {}


	void D3D12VertexBuffer::CreateViews(size_t numElements, size_t elementSize)
	{
		m_VertexCount = numElements;
		m_VertexStride = elementSize;

		m_VertexBufferView.BufferLocation = m_InteralResource->GetGPUVirtualAddress();
		m_VertexBufferView.SizeInBytes = static_cast<unsigned int>(m_VertexCount * m_VertexStride);
		m_VertexBufferView.StrideInBytes = static_cast<unsigned int>(m_VertexStride);
	}

	D3D12_VERTEX_BUFFER_VIEW D3D12VertexBuffer::GetVertexBufferView() const
	{
		return m_VertexBufferView;
	}

	size_t D3D12VertexBuffer::GetVertexCount() const
	{
		return m_VertexCount;
	}

	size_t D3D12VertexBuffer::GetVertexStride() const
	{
		return m_VertexStride;
	}
}