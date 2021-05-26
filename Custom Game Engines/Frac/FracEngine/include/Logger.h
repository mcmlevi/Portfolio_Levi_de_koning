#pragma once
#include <spdlog/spdlog.h>
#include <memory>
#include <spdlog/fmt/bundled/printf.h>
#include <string>

namespace Frac
{

	/// <summary>
	/// brief The level to log for example if set to Error: Debug, Info and warning messages will not be displayed
	/// </summary>
	enum eLogLevel
	{
		Debug = 1,
		Info = 2,
		Warn = 3,
		Error = 4,
		Critical = 5,
		Off = 6
	};

	class Logger
	{

	public:

		///brief Get the message logger instance
		///return The message logger instance
		static Logger& GetInstance();

		///brief Log a warning using the printf format
		///tparam Args printf style arguments
		///param message the message to log uses the printf format
		 //param arguments the message arguments 
		template<typename... Args>
		void LogWarning(const std::string& message, Args&&... arguments);

		///brief Log a info message using the printf format
		///tparam Args printf style arguments
		///param message the message to log uses the printf format
		///param arguments the message arguments
		template<typename... Args>
		void LogInfo(const std::string& message, Args&&... arguments);

		///brief Log a debug message using the printf format
		///tparam Args printf style arguments
		///param message the message to log uses the printf format
		///param arguments the message arguments
		template<typename... Args>
		void LogDebug(const std::string& message, Args&&... arguments);

		///brief Log a error message using the printf format
		///tparam Args printf style arguments
		///param message the message to log uses the printf format
		///param arguments the message arguments
		template<typename... Args>
		void LogError(const std::string& message, Args&&... arguments);

		///brief Log a critical message using the printf format
		///tparam Args printf style arguments
		///param message the message to log uses the printf format
		///param arguments the message arguments
		template<typename... Args>
		void LogCritical(const std::string& message, Args&&... arguments);

		///brief Initialize the message / file logger it will generate a logfile on windows
		///param filepath the filepath to store the logfile
		///param fileLogLevel the log level for in the file default set to info
		void Initialize(const std::string& filepath, eLogLevel fileLogLevel = eLogLevel::Info);

		///brief set the file log level to a specified log level.
		///param logLevel the level to log for example if set to Error: Debug, Info and warning messages will not be displayed.
		void SetFileLogLevel(eLogLevel logLevel);

		///brief Needs to be called when the engine closes to properly save the log file and close spdlog on windows.
		void Shutdown();

	private:

		template <typename ... Args>
		std::string FormatSpdLogMessage(const std::string& message, Args&&... arguments);

#ifdef PLATFORM_WINDOWS
		std::shared_ptr<spdlog::logger> m_fileLogger{ nullptr };
#endif

		const std::string m_fileLoggerName{ "FileLogger" };
		eLogLevel m_FileLevel{};

	};

	/// <summary>
	/// Logger Template Functions
	/// </summary>
	template <typename ... Args>
	void Logger::LogWarning(const std::string& message, Args&&... arguments)
	{
		const std::string formattedMessage{ Frac::Logger::FormatSpdLogMessage(message,std::forward<Args>(arguments)...) };
		spdlog::warn(formattedMessage);
#ifdef PLATFORM_WINDOWS
		m_fileLogger->warn(formattedMessage);
#endif
	}

	template <typename ... Args>
	void Logger::LogInfo(const std::string& message, Args&&... arguments)
	{
		const std::string formattedMessage{ Frac::Logger::FormatSpdLogMessage(message,std::forward<Args>(arguments)...) };
		spdlog::info(formattedMessage);
#ifdef PLATFORM_WINDOWS
		m_fileLogger->info(formattedMessage);
#endif
	}

	template <typename ... Args>
	void Logger::LogDebug(const std::string& message, Args&&... arguments)
	{
		const std::string formattedMessage{ Frac::Logger::FormatSpdLogMessage(message,std::forward<Args>(arguments)...) };
		spdlog::debug(formattedMessage);
#ifdef PLATFORM_WINDOWS
		m_fileLogger->debug(formattedMessage);
#endif
	}

	template <typename ... Args>
	void Logger::LogError(const std::string& message, Args&&... arguments)
	{
		const std::string formattedMessage{ Frac::Logger::FormatSpdLogMessage(message,std::forward<Args>(arguments)...) };
		spdlog::error(formattedMessage);
#ifdef PLATFORM_WINDOWS
		m_fileLogger->error(formattedMessage);
#endif
	}

	template <typename ... Args>
	void Logger::LogCritical(const std::string& message, Args&&... arguments)
	{
		const std::string formattedMessage{ Frac::Logger::FormatSpdLogMessage(message,std::forward<Args>(arguments)...) };
		spdlog::critical(formattedMessage);
#ifdef PLATFORM_WINDOWS
		m_fileLogger->critical(formattedMessage);
#endif
	}

	template <typename ... Args>
	std::string Logger::FormatSpdLogMessage(const std::string& message, Args&&... arguments)
	{
		return std::string{ fmt::sprintf(message,std::forward<Args>(arguments)...) };
	}
}

#define LOGDEBUG Frac::Logger::GetInstance().LogDebug
#define LOGINFO Frac::Logger::GetInstance().LogInfo
#define LOGWARNING Frac::Logger::GetInstance().LogWarning
#define LOGERROR Frac::Logger::GetInstance().LogError
#define LOGCRITICAL Frac::Logger::GetInstance().LogCritical


