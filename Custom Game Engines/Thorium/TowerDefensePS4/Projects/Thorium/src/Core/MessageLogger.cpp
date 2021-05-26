#include "Thpch.h"
#include <Core/MessageLogger.h>
#ifdef PLATFORM_WIN64
#include <spdlog/async.h>
#include "spdlog/sinks/basic_file_sink.h"
#elif PLATFORM_PS4
#endif


namespace Th 
{

	MessageLogger& Th::MessageLogger::GetInstance()
	{
		static MessageLogger logger{};
		return logger;
	}

	void MessageLogger::Initialize(const std::string& filepath, eLogLevel logLevel, eLogLevel fileLogLevel)
	{
		m_ConsoleLevel = logLevel;
		m_FileLevel = fileLogLevel;
#ifdef PLATFORM_WIN64
		spdlog::set_level(static_cast<spdlog::level::level_enum>(logLevel));
		m_fileLogger = spdlog::basic_logger_mt<spdlog::async_factory>("FileLogger", filepath);
		m_fileLogger->set_level(static_cast<spdlog::level::level_enum>(fileLogLevel));
#elif PLATFORM_PS4
		static_cast<void>(filepath);
#endif
		EventMessenger::GetInstance().AddMessenger<const std::string&>("ConsoleMessage");
		
		
	}

	void MessageLogger::SetGlobalLogLevel(eLogLevel logLevel)
	{
		m_FileLevel = logLevel;
		m_ConsoleLevel = logLevel;
#ifdef PLATFORM_WIN64
		spdlog::set_level(static_cast<spdlog::level::level_enum>(m_ConsoleLevel));
		m_fileLogger->set_level(static_cast<spdlog::level::level_enum>(m_FileLevel));
#endif

	}

	void MessageLogger::SetConsoleLogLevel(eLogLevel logLevel)
	{
		m_ConsoleLevel = logLevel;
#ifdef PLATFORM_WIN64
		spdlog::set_level(static_cast<spdlog::level::level_enum>(m_ConsoleLevel));
#endif
	}

	void MessageLogger::SetFileLogLevel(eLogLevel logLevel)
	{
		m_FileLevel = logLevel;
#ifdef PLATFORM_WIN64
		m_fileLogger->set_level(static_cast<spdlog::level::level_enum>(m_FileLevel));
#endif
	}

	void MessageLogger::Shutdown()
	{
#ifdef PLATFORM_WIN64
		m_fileLogger->flush();
		spdlog::shutdown();
#elif PLATFORM_PS4
		
#endif

	}
}
