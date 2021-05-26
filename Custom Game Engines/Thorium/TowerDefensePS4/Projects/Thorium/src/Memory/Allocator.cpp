#include <Thpch.h>
#include "Memory/Allocator.h"
#include <cassert> //assert

namespace Th
{
    Allocator::Allocator(const std::size_t totalSize)
    {
        m_totalSize = totalSize;
        m_used = 0;
    }

    Allocator::~Allocator()
    {
        m_totalSize = 0;
    }

    std::size_t Allocator::GetId() const
    {
        return m_AllocatorId;
    }
}
