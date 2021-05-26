#include <Thpch.h>
#include <Core/Resource.h>

std::string Th::Resource::GetID()
{
	return m_resourcePath;
}

Th::eResourceType Th::Resource::GetType()
{
	return m_type;
}

Th::Resource::Resource(const std::string& filepath, const eResourceType type ):m_resourcePath{ filepath },m_type{type}
{
}
