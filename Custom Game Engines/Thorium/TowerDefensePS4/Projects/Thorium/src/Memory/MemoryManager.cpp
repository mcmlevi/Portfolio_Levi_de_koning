#include <Thpch.h>
#include <Memory/MemoryManager.h>

namespace Th
{
	MemoryManager& Th::MemoryManager::GetInstance()
	{
		static MemoryManager manager{};
		return manager;
	}

	std::size_t MemoryManager::GetAllocatorId(MemoryArea allocator)
	{
		return m_memory[allocator]->GetId();
	}

	void* MemoryManager::Allocate(MemoryArea allocator, const std::size_t size, const std::size_t alignment)
	{
		return m_memory[allocator]->Allocate(size,alignment);
	}

	void MemoryManager::Free(void* memory, MemoryArea allocator)
	{
		m_memory[allocator]->Free(memory);
	}

	void MemoryManager::Reset(MemoryArea allocator)
	{
		m_memory[allocator]->Reset();
	}

	void MemoryManager::Shutdown()
	{
		for (auto& memory : m_memory)
		{
			memory.second->~Allocator(); //TODO: Check reason for odd mannered destruction.
			memory.second = nullptr;
		}
	}
}