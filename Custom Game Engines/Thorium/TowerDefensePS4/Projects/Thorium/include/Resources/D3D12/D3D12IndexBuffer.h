#pragma once
#include <Resources/D3D12/D3D12Buffer.h>

namespace Th
{
	/// <summary>
	/// A D3D12 resource buffer used to store indicies in.
	/// </summary>
	class D3D12IndexBuffer : public D3D12Buffer
	{
	public:
		D3D12IndexBuffer(const std::wstring& name = L"");
		~D3D12IndexBuffer() = default;

		virtual void CreateViews(size_t numElements, size_t elementSize) override;

		size_t GetNumIndicies() const;
		DXGI_FORMAT GetIndexFormat() const;
		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

	private:
		size_t m_IndexCount;
		DXGI_FORMAT m_IndexFormat;
		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
	};
}