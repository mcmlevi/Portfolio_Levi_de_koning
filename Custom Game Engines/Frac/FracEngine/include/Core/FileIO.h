#pragma once

#include "Core/ECSWrapper.h"

#ifdef PLATFORM_SWITCH

#endif // PLATFORM_SWITCH

namespace Frac 
{
	/** 
	* This class provides a filesystem for reading and writing files on both Windows and Switch platforms
	*/
	class FileIO
	{
	public:
		FileIO();
		~FileIO() = default;

		/** 
		* Reads a file from the filesystem
		* This function is used to read a file from filesystem.
		* @param a_fileName: the filename of the file you want to read data from. Supports [Wildcards] to find the right directory
		* @return: A string with the data from that file
		*/
		static std::string Read(const std::string& a_fileName);

		/**
		* Writes to a file from the filesystem
		* This function is used to write data to a file from the filesystem.
		* Overwrites already existing files and creates new files if they don't exist yet
		* @param a_fileName: the filename of the file you want to read data from. Supports [Wildcards] to find the right directory
		* @param a_content: the data you want to write to the file
		* @return: The state of success
		*/
		static bool Write(const std::string& a_filename, const std::string& a_content);

		/**
		* @param a_wildcard the [Wildcard]
		* @return The filepath
		*/
		static std::string GetPathFromWildcard(const std::string & a_wildcard);
		static std::string GetNameFromFilePath(const std::string& filePath);

	private:
		static std::string GetPath(const std::string& a_path);
		static std::string StringReplace(const std::string& a_subject, const std::string& a_search, const std::string& a_replace);
		static void AddWildcard(const std::string& a_wildcard, const std::string a_filepath);
		
#ifdef PLATFORM_SWITCH
	
#endif // PLATFORM_SWITCH

		static std::string s_rootAssetsDirectory;
		static std::vector<std::pair<std::string, std::string>> s_wildCards;
	};
} // namespace Frac