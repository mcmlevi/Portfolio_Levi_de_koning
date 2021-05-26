#include <Thpch.h>
#include "Memory/CAllocator.h"
#include <stdlib.h>     /* malloc, free */

namespace Th
{
	CAllocator::CAllocator() : Allocator(0) {}

	void* CAllocator::Allocate(const std::size_t size, const std::size_t alignment) 
	{
		static_cast<void>(alignment);
		return malloc(size);
	}

	void CAllocator::Free(void* ptr) 
	{
		free(ptr);
	}
}

