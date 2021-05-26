#include <Thpch.h>
#include <Resources/D3D12/D3D12UploadBuffer.h>
#include <Core/MessageLogger.h>
#include <Utility/D3DErrorHandler.h>

namespace Th
{
	void D3D12UploadBuffer::Initialize(D3D12Device& device, size_t pageSize /*= _2MB*/)
	{
		m_Device = device;
		m_PageSize = pageSize;
	}

	void D3D12UploadBuffer::Destroy()
	{
		Reset();
	}

	Th::D3D12UploadBuffer::Allocation D3D12UploadBuffer::Allocate(size_t sizeInBytes, size_t alignment)
	{
		if (sizeInBytes > m_PageSize)
		{
			LOGERROR("Cannot create an upload buffer with a larger size than the memory block may exceed!");
			throw std::bad_alloc();
		}

		if (m_CurrentPage == nullptr)
		{
			m_CurrentPage = RequestPage(alignment);
		}

		Allocation allocation;
		m_CurrentPage->Allocate(allocation, sizeInBytes, alignment);

		//Check if allocation on the current page has succeeded if not, request a new page that has enough space.
		if (!IsAllocationValid(allocation))
		{
			m_CurrentPage = RequestPage(alignment);
			m_CurrentPage->Allocate(allocation, sizeInBytes, alignment);

#if defined _DEBUG //If this is the case in debug it is also in release. Therefor test run in debug and run for release.
			if (!IsAllocationValid(allocation))
			{
				LOGERROR("The page size is too small for this resource to be allocated!");
			}
#endif
		}

		return allocation;
	}

	void D3D12UploadBuffer::Reset()
	{
		m_CurrentPage = nullptr;
		m_AvailablePages = m_PagePool;

		for (auto page : m_AvailablePages)
		{
			page->Reset();
		}
	}

	size_t D3D12UploadBuffer::GetPageSize() const
	{
		return m_PageSize;
	}

	Th::D3D12UploadBuffer::Page* D3D12UploadBuffer::RequestPage(size_t alignment)
	{
		Page* page = nullptr;

		if (!m_AvailablePages.empty())
		{
			page = m_AvailablePages.front();
			m_AvailablePages.pop_front();
		}
		else
		{
			page = new Page(m_Device, m_PageSize);
			m_PagePool.push_back(page);
		}

		return page;
	}

	bool D3D12UploadBuffer::IsAllocationValid(const Allocation& allocation) const
	{
		return allocation.CPUAddress != nullptr && allocation.GPUAddress != D3D12_GPU_VIRTUAL_ADDRESS(0);
	}

	D3D12UploadBuffer::Page::Page(D3D12Device& device, size_t sizeInBytes) :
		m_GPUAddress(D3D12_GPU_VIRTUAL_ADDRESS(0)),
		LinearAllocator(sizeInBytes)
	{
		D3D_CHECK(device.Get()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(sizeInBytes),
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_Resource)));

		m_Resource->SetName(L"Upload buffer (page)");
		m_GPUAddress = m_Resource->GetGPUVirtualAddress();
	}

	D3D12UploadBuffer::Page::~Page()
	{
		m_GPUAddress = D3D12_GPU_VIRTUAL_ADDRESS(0);
	}

	void D3D12UploadBuffer::Page::Allocate(Allocation& allocation, size_t sizeInBytes, size_t alignment)
	{
		size_t offset = m_offset; //Cache the aligned size before allocating a new cpu adress since that will move the offset onwards.
		allocation.CPUAddress = LinearAllocator::Allocate(sizeInBytes, alignment);
		allocation.GPUAddress = allocation.CPUAddress != nullptr ? m_GPUAddress + offset : D3D12_GPU_VIRTUAL_ADDRESS(0);
	}

	void D3D12UploadBuffer::Page::Reset()
	{
		LinearAllocator::Reset();
	}
}