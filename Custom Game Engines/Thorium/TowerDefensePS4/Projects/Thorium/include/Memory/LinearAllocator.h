#pragma once

#include "Memory/Allocator.h"

namespace Th
{
	class LinearAllocator : public Allocator 
	{
	protected:
		void* m_start_ptr = nullptr;
		std::size_t m_offset;

	public:
		LinearAllocator(const std::size_t totalSize);
		virtual ~LinearAllocator();

		virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) override;
		virtual void Free(void* ptr) override;
		virtual void Init() override;
		virtual void Reset() override;

	private:
		LinearAllocator(LinearAllocator &linearAllocator);
	};
}
