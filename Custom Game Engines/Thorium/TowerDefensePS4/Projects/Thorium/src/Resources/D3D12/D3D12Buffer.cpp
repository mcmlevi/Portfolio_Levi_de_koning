#include <Thpch.h>
#include <Resources/D3D12/D3D12Buffer.h>

namespace Th
{
	D3D12Buffer::D3D12Buffer(const std::wstring& name /*= L""*/) 
		: D3D12Resource(name) {}

	D3D12Buffer::D3D12Buffer(const D3D12_RESOURCE_DESC& desc, size_t numElements, size_t elementSize, const std::wstring& name /*= L""*/)
		: D3D12Resource(desc, nullptr, name) {}
}