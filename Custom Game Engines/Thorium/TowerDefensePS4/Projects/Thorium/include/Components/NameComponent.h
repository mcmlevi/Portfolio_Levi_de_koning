#pragma once
#include <string>
namespace Th
{
	class NameComponent
	{
	public:
		NameComponent() = default;
		NameComponent(const std::string& name) { Name = name; }
		
		std::string Name;
	};
}
