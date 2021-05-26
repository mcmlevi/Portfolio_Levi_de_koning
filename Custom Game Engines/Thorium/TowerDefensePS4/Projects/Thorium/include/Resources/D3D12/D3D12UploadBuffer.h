#pragma once
#include <Graphics/D3D12/D3D12Device.h>
#include <Memory/MemoryDefines.h>
#include <Memory/MemoryManager.h>
#include <Memory/LinearAllocator.h>
#include <deque>
#include <d3d12.h>

namespace Th
{
	/// <summary>
	/// The upload buffer manages the data allocation from the CPU to the GPU using a linear allocator memory behaviour.
	/// </summary>
	class D3D12UploadBuffer
	{
	public:
		/// <summary>
		/// Used to retrieve the allocation data upon allocation.
		/// </summary>
		struct Allocation
		{
			void* CPUAddress;
			D3D12_GPU_VIRTUAL_ADDRESS GPUAddress;
		};

	private:
		/// <summary>
		/// Represents a memory block, which is part of the uploadbuffer.
		/// </summary>
		struct Page final : public LinearAllocator
		{
		public:
			Page(D3D12Device& device, size_t pageSize);
			virtual ~Page();

			void Allocate(Allocation& allocation, size_t sizeInBytes, size_t alignment);
			void Reset() override;

		private:
			D3D12_GPU_VIRTUAL_ADDRESS m_GPUAddress;
			Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource;
		};

	public:
		D3D12UploadBuffer() = default;
		virtual ~D3D12UploadBuffer() = default;

		void Initialize(D3D12Device& device, size_t pageSize = _2MB);
		void Destroy();

		Allocation Allocate(size_t sizeInBytes, size_t alignment);
		void Reset();

		size_t GetPageSize() const;

	private:
		Page* RequestPage(size_t alignment);
		bool IsAllocationValid(const Allocation& allocation) const;

		std::deque<Page*> m_PagePool;
		std::deque<Page*> m_AvailablePages;

		Page* m_CurrentPage;
		size_t m_PageSize;
		D3D12Device m_Device;
	};
}