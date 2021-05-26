#include "pch.h"
#include "Logger.h"
#include <spdlog/async.h>
#include "spdlog/sinks/basic_file_sink.h"

namespace Frac
{

	Logger& Frac::Logger::GetInstance()
	{
		static Logger logger{};
		return logger;
	}

	void Logger::Initialize(const std::string& filepath, eLogLevel fileLogLevel)
	{
		m_FileLevel = fileLogLevel;

#ifdef PLATFORM_WINDOWS
		m_fileLogger = spdlog::basic_logger_mt<spdlog::async_factory>("FileLogger", filepath);
		m_fileLogger->set_level(static_cast<spdlog::level::level_enum>(fileLogLevel));
#endif
	}

	void Logger::SetFileLogLevel(eLogLevel logLevel)
	{
		m_FileLevel = logLevel;

#ifdef PLATFORM_WINDOWS
		m_fileLogger->set_level(static_cast<spdlog::level::level_enum>(m_FileLevel));
#endif
	}

	void Logger::Shutdown()
	{
#ifdef PLATFORM_WINDOWS
		m_fileLogger->flush();
#endif

		spdlog::shutdown();
	}

	
}