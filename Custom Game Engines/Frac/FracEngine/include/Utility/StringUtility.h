#pragma once
#include <string>

namespace Frac
{
	namespace Utility
	{
		extern unsigned long long StringHash(const std::string& str);
		extern std::string StringReplace(const std::string& subject, const std::string& search, const std::string& replace);
		extern bool StringStartsWith(const std::string& subject, const std::string& prefix);
		extern bool StringEndsWith(const std::string& subject, const std::string& suffix);
		extern void StringCapitalizeFirst(std::string& str);
		extern std::string UnicodeToUTF8(unsigned int codepoint);
		extern std::string ToString(float f, int precision);
		extern std::string ToHexString(int number);
	}
}
