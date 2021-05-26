#pragma once
#include <Resources/D3D12/D3D12Resource.h>

namespace Th
{
	/// <summary>
	/// The base class for a resource buffer in d3d12.
	/// </summary>
	class D3D12Buffer : public D3D12Resource
	{
	public:
		D3D12Buffer(const std::wstring& name = L"");
		D3D12Buffer(const D3D12_RESOURCE_DESC& desc, size_t numElements, size_t elementSize, const std::wstring& name = L"");
		virtual ~D3D12Buffer() = default;

		/// <summary>
		/// Create the views for the buffer resource used to get the GPU virtual address.
		/// Used by the commandlist when setting the buffer contents.
		/// </summary>
		/// <param name="numElements">The number of elements the buffer will contain.</param>
		/// <param name="elementSize">The size of each element in the buffer.</param>
		virtual void CreateViews(size_t numElements, size_t elementSize) = 0;
	};
}