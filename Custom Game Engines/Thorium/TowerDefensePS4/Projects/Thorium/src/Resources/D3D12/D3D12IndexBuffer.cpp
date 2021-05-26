#include <Thpch.h>
#include <Resources/D3D12/D3D12IndexBuffer.h>

namespace Th
{
	D3D12IndexBuffer::D3D12IndexBuffer(const std::wstring& name /*= L""*/)
		: D3D12Buffer(name),
		m_IndexCount(0),
		m_IndexFormat(DXGI_FORMAT::DXGI_FORMAT_R16_UINT),
		m_IndexBufferView({}) { }

	void D3D12IndexBuffer::CreateViews(size_t numElements, size_t elementSize)
	{
		assert(elementSize == 2 || elementSize == 4 && "Indices must be 16 or 32-bit integers!");

		m_IndexCount = numElements;
		m_IndexFormat = (elementSize == 2) ? DXGI_FORMAT::DXGI_FORMAT_R16_UINT : DXGI_FORMAT::DXGI_FORMAT_R32_UINT;

		m_IndexBufferView.BufferLocation = m_InteralResource->GetGPUVirtualAddress();
		m_IndexBufferView.Format = m_IndexFormat;
		m_IndexBufferView.SizeInBytes = static_cast<unsigned int>(numElements * elementSize);
	}

	size_t D3D12IndexBuffer::GetNumIndicies() const
	{
		return m_IndexCount;
	}

	DXGI_FORMAT D3D12IndexBuffer::GetIndexFormat() const
	{
		return m_IndexFormat;
	}

	D3D12_INDEX_BUFFER_VIEW D3D12IndexBuffer::GetIndexBufferView() const
	{
		return m_IndexBufferView;
	}

}