#pragma once

#include <cstddef> // size_t

namespace Th
{
    /// <summary>
    /// Allocator base class for the memory manager.
    /// </summary>
    class Allocator 
    {
    private:
        std::size_t m_AllocatorId;
        static std::size_t s_AllocatorCount;

    protected:
        std::size_t m_totalSize;
        std::size_t m_used;   
        std::size_t m_peak;

    public:
        Allocator(const std::size_t totalSize);
        virtual ~Allocator();

        virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) = 0;
        virtual void Free(void* ptr) = 0;
        virtual void Init() = 0;
        virtual void Reset() = 0;

        std::size_t GetId() const;
    };
}