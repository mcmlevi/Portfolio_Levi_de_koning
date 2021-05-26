#pragma once
#include <Resources/D3D12/D3D12Buffer.h>

namespace Th
{
	/// <summary>
	/// A D3D12 resource buffer used to store vertices in.
	/// </summary>
	class D3D12VertexBuffer : public D3D12Buffer
	{
	public:
		D3D12VertexBuffer(const std::wstring& name = L"");
		~D3D12VertexBuffer() = default;

		virtual void CreateViews(size_t numElements, size_t elementSize) override;

		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;

		size_t GetVertexCount() const;
		size_t GetVertexStride() const;

	private:
		size_t m_VertexCount;
		size_t m_VertexStride;

		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
	};
}