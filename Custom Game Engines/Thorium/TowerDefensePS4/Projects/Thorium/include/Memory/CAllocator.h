#ifndef CALLOCATOR_H
#define CALLOCATOR_H

#include <Memory/Allocator.h>

namespace Th
{
	/// <summary>
	/// The CAllocator uses the malloc and free methods from the C language in order to allocate memory and free memory.
	/// </summary>
	class CAllocator : public Allocator 
	{
	public:
		CAllocator();
		virtual ~CAllocator() = default;

		virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) override;
		virtual void Free(void* ptr) override;
	};
}

#endif /* CALLOCATOR_H */

