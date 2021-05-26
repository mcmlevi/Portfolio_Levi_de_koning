#pragma once

#include "Memory/Allocator.h"
#include "Memory/SinglyLinkedList.h"

namespace Th
{
    class FreeListAllocator : public Allocator 
    {
    public:
        enum PlacementPolicy 
        {
            FIND_FIRST,
            FIND_BEST
        };
    protected:
        void* m_start_ptr = nullptr;
	
    private:
        struct FreeHeader 
        {
            std::size_t blockSize;
        };
        
        struct AllocationHeader 
        {
            std::size_t blockSize;
            char padding;
        };
    
        typedef SinglyLinkedList<FreeHeader>::Node Node;

        PlacementPolicy m_pPolicy;
        SinglyLinkedList<FreeHeader> m_freeList;

    public:
        FreeListAllocator(const std::size_t totalSize, const PlacementPolicy pPolicy);
        virtual ~FreeListAllocator();

        virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) override;
        virtual void Free(void* ptr) override;
        virtual void Init() override;
        virtual void Reset() override;

    private:
        FreeListAllocator(FreeListAllocator &freeListAllocator);

        void Coalescence(Node* prevBlock, Node * freeBlock);

        void Find(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode);
        void FindBest(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode);
        void FindFirst(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode);
    };
}
