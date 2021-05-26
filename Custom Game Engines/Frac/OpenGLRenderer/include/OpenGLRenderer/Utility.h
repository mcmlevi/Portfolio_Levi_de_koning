#pragma once

namespace TOR
{
#if defined (PLATFORM_SWITCH)

#endif

	/// Read an entire text file into a string. Only use for small files.
	///
	/// @filename   Full path to the file
	///
	std::string ReadFileToString(const std::string& filename);

	/// Read an entire text file into a char buffer.
	///
	/// @filename   Full path to the file
	///
	bool ReadFileToBuffer(const std::string& filename, unsigned char*& buffer, int& length);
	unsigned long long StringHash(const std::string& str);
	std::string StringReplace(const std::string& subject, const std::string& search, const std::string& replace);
	bool StringStartsWith(const std::string& subject, const std::string& prefix);
	bool StringEndsWith(const std::string& subject, const std::string& suffix);
	void StringCapitalizeFirst(std::string& str);
	std::string UnicodeToUTF8(unsigned int codepoint);
	std::string ToString(float f, int precision);
	std::string ToHexString(int number);
}