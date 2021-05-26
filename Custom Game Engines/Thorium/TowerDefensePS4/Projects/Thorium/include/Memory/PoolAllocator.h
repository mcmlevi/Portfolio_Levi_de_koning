#pragma once
#include "Memory/Allocator.h"
#include "Memory/StackLinkedList.h"

namespace Th
{
    class PoolAllocator : public Allocator 
    {
    private:
        struct  FreeHeader {};
        using Node = StackLinkedList<FreeHeader>::Node;
        StackLinkedList<FreeHeader> m_freeList;

   
        std::size_t m_chunkSize;
    public:
        PoolAllocator(const std::size_t totalSize, const std::size_t chunkSize);

        virtual ~PoolAllocator();

        virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) override;
        virtual void Free(void* ptr) override;
        virtual void Init() override;
        virtual void Reset() override;

    protected:
        void* m_start_ptr = nullptr;
    
    private:
        PoolAllocator(PoolAllocator &poolAllocator);
    };
}