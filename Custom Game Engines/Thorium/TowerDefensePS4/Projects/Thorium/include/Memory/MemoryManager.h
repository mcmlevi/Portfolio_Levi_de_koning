#pragma once

#include <cassert>
#include <unordered_map>

#include <Memory/Allocator.h>
#include <Memory/ThLinearAllocator.h>

namespace Th
{
	typedef void* MemoryArea;

	class MemoryManager
	{
	public:
		MemoryManager() = default;
		~MemoryManager() = default;

		/**
		 * \brief Get the instance of the memory manager
		 * \return Return the singleton instance
		 */
		static MemoryManager& GetInstance();
		/**
		 * \brief Function To create a new memory behavior. This will automatically allocate memory.
		 * \tparam Behaviour the Behavior you want to use linear,pool,stack or free list allocating behavior.
		 * \param totalSize the size you want the new memory behavior to have.
		 * \param arguments extra parameters needed for the behavior for example chunck size.
		 * \return return a memoryArea that serves as a key to the newly created behavior.
		 */
		template <typename Behaviour, typename... Args>
		MemoryArea CreateMemoryAllocator(const std::size_t totalSize,Args&&... arguments);
	
		/// <summary>
		/// Retrieves the allocator id of the given memory block.
		/// </summary>
		/// <param name="allocator"></param>
		/// <returns>The id of the allocator of the memory area.</returns>
		std::size_t GetAllocatorId(MemoryArea allocator);

		/**
		 * \brief Allocate memory of n-bytes
		 * \param size number of bytes to allocate
		 * \param allocator key to the allocator
		 * \return void* to the memory adress of the new allocated region
		 */
		void* Allocate(MemoryArea allocator,const std::size_t size, const std::size_t alignment  = 0);
		
		/**
		 * \brief Free a chunk of memory.
		 * \param memory ptr to the adress to be freed.
		 * \param allocator key to the allocator.
		 */
		void Free(void* memory, MemoryArea allocator);
		
		/** 
		 * \brief Reset the memory to be used again.
		 * \param allocator key to the allocator.
		 */
		void Reset(MemoryArea allocator);

		/// <summary>
		/// Cleans the memory of the allocations there were made.
		/// </summary>
		void Shutdown();

	private:
		std::unordered_map<MemoryArea, Allocator*> m_memory;
	};


	template <typename Behaviour, typename... Args>
	MemoryArea MemoryManager::CreateMemoryAllocator(const std::size_t totalSize,Args&&... arguments)
	{
#if defined _DEBUG
		const bool isBaseOf = std::is_base_of<Allocator, Behaviour>::value;
		assert(isBaseOf);
#endif		
		Allocator* allocator = new Behaviour(totalSize, arguments...);
		allocator->Init();

		MemoryArea startMemory = static_cast<MemoryArea>(allocator);
		m_memory.insert({ startMemory,  allocator });
		return startMemory;
	}
}

