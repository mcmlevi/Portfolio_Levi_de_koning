#pragma once

#ifdef PLATFORM_WIN64

#pragma warning(push,0)
#pragma warning(disable: 6387 26451 26495 26812)
#include <spdlog/spdlog.h>
#include <memory>
#include <spdlog/fmt/bundled/printf.h>
#pragma warning(pop)

#elif PLATFORM_PS4
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
#endif

#include <string>
#include <Events/EventMessenger.h>

namespace Th
{
	/**
	 * \brief The level to log for example if set to Error: Debug, Info and warning messages will not be displayed
	 */
	enum eLogLevel
	{
		Debug = 1,
		Info = 2,
		Warn = 3,
		Error = 4,
		Critical = 5,
		Off = 6
	};
	
	class MessageLogger
	{
	public:
		/**
		 * \brief Get the message logger instance
		 * \return The message logger instance
		 */
		static MessageLogger& GetInstance();
		/**
		 * \brief Log a warning using the printf format 
		 * \tparam Args printf style arguments 
		 * \param message the message to log uses the printf format
		 * \param arguments the message arguments
		 */
		template<typename... Args>
		void LogWarning(const std::string& message, Args&&... arguments);
		/**
		* \brief Log a info message using the printf format
		* \tparam Args printf style arguments
		* \param message the message to log uses the printf format
		* \param arguments the message arguments
		*/
		template<typename... Args>
		void LogInfo(const std::string& message, Args&&... arguments);
		/**
		 * \brief Log a debug message using the printf format
		 * \tparam Args printf style arguments
		 * \param message the message to log uses the printf format
		 * \param arguments the message arguments
		 */
		template<typename... Args>
		void LogDebug(const std::string& message, Args&&... arguments);
		/**
		 * \brief Log a error message using the printf format
		 * \tparam Args printf style arguments
		 * \param message the message to log uses the printf format
		 * \param arguments the message arguments
		 */
		template<typename... Args>
		void LogError(const std::string& message, Args&&... arguments);
		/**
		 * \brief Log a critical message using the printf format
		 * \tparam Args printf style arguments
		 * \param message the message to log uses the printf format
		 * \param arguments the message arguments
		 */
		template<typename... Args>
		void LogCritical(const std::string& message, Args&&... arguments);
		/**
		 * \brief Initialize the message / file logger it will generate a logfile on windows and do nothing file logging wise on the ps4
		 * \param filepath the filepath to store the logfile
		 * \param logLevel the log level for the console default to info
		 * \param fileLogLevel the log level for in the file default set to info
		 */
		void Initialize(const std::string& filepath, eLogLevel logLevel = eLogLevel::Info, eLogLevel fileLogLevel = eLogLevel::Info);
		/**
		 * \brief Set the file and console log level to a specified log level.
		 * \param logLevel the level to log for example if set to Error: Debug, Info and warning messages will not be displayed.
		 */
		void SetGlobalLogLevel(eLogLevel logLevel);
		/**
		 * \brief set the console log level to a specified log level.
		 * \param logLevel the level to log for example if set to Error: Debug, Info and warning messages will not be displayed.
		 */
		void SetConsoleLogLevel(eLogLevel logLevel);
		/**
		 * \brief set the file log level to a specified log level.
		 * \param logLevel the level to log for example if set to Error: Debug, Info and warning messages will not be displayed.
		 */
		void SetFileLogLevel(eLogLevel logLevel);
		/**
		 * \brief Needs to be called when the engine closes to properly save the log file and close spdlog on windows.
		 */
		void Shutdown();
	private:
#ifdef PLATFORM_WIN64
		template <typename ... Args>
		std::string FormatSpdLogMessage(const std::string& message, Args&&... arguments);
		std::shared_ptr<spdlog::logger> m_fileLogger{ nullptr };
		const std::string m_fileLoggerName{ "FileLogger" };
#elif PLATFORM_PS4
#endif
		eLogLevel m_ConsoleLevel{};
		eLogLevel m_FileLevel{};
	};

	template <typename ... Args>
	void MessageLogger::LogWarning(const std::string& message, Args&&... arguments)
	{
#ifdef PLATFORM_WIN64
		const std::string formattedMessage{ FormatSpdLogMessage(message,std::forward<Args>(arguments)...) };
		spdlog::warn(formattedMessage);
		m_fileLogger->warn(formattedMessage);
		if (m_ConsoleLevel <= eLogLevel::Warn)
		{
			const std::string warningString{ "[warning] " + formattedMessage + "\n" };
			Th::EventMessenger::GetInstance().EvokeMessenger("ConsoleMessage", warningString);
		}
#elif PLATFORM_PS4
		if(m_ConsoleLevel <= eLogLevel::Warn)
		{
			const std::string formattedMessage{"[warning]" + message + "\n" };
			printf(formattedMessage.c_str(), std::forward<Args>(arguments)...);
		}
			
#endif
	}

	template <typename ... Args>
	void MessageLogger::LogInfo(const std::string& message, Args&&... arguments)
	{
#ifdef PLATFORM_WIN64
		const std::string formattedMessage{ FormatSpdLogMessage(message,std::forward<Args>(arguments)...) };
		spdlog::info(formattedMessage);
		m_fileLogger->info(formattedMessage);
		if (m_ConsoleLevel <= eLogLevel::Info)
		{
			const std::string infoString{ "[info] " + formattedMessage + "\n" };
			Th::EventMessenger::GetInstance().EvokeMessenger("ConsoleMessage", infoString);
		}
#elif PLATFORM_PS4
		if (m_ConsoleLevel <= eLogLevel::Info)
		{
			const std::string formattedMessage{ "[info]" + message + "\n" };
			printf(formattedMessage.c_str(), std::forward<Args>(arguments)...);
		}

#endif
	}

	template <typename ... Args>
	void MessageLogger::LogDebug(const std::string& message, Args&&... arguments)
	{
#ifdef PLATFORM_WIN64
		const std::string formattedMessage{ FormatSpdLogMessage(message,std::forward<Args>(arguments)...)};
		spdlog::debug(formattedMessage);
		m_fileLogger->debug(formattedMessage);
		if (m_ConsoleLevel <= eLogLevel::Debug)
		{
			const std::string debugString{ "[debug] " + formattedMessage + "\n" };
			Th::EventMessenger::GetInstance().EvokeMessenger("ConsoleMessage", debugString);
		}
#elif PLATFORM_PS4
		if (m_ConsoleLevel <= eLogLevel::Debug)
		{
			const std::string formattedMessage{ "[debug]" + message + "\n" };
			printf(formattedMessage.c_str(), std::forward<Args>(arguments)...);
		}

#endif
	}

	template <typename ... Args>
	void MessageLogger::LogError(const std::string& message, Args&&... arguments)
	{
#ifdef PLATFORM_WIN64
		const std::string formattedMessage{ FormatSpdLogMessage(message,std::forward<Args>(arguments)...) };
		spdlog::error(formattedMessage);
		m_fileLogger->error(formattedMessage);
		if (m_ConsoleLevel <= eLogLevel::Error)
		{
			const std::string errorString{ "[error] " + formattedMessage + "\n" };
			Th::EventMessenger::GetInstance().EvokeMessenger("ConsoleMessage", errorString);
		}
#elif PLATFORM_PS4
		if (m_ConsoleLevel <= eLogLevel::Error)
		{
			const std::string formattedMessage{ "[error]" + message + "\n" };
			printf(formattedMessage.c_str(), std::forward<Args>(arguments)...);
		}

#endif
	}

	template <typename ... Args>
	void MessageLogger::LogCritical(const std::string& message, Args&&... arguments)
	{
#ifdef PLATFORM_WIN64
		const std::string formattedMessage{ FormatSpdLogMessage(message,std::forward<Args>(arguments)...) };
		spdlog::critical(formattedMessage);
		m_fileLogger->critical(formattedMessage);
		if (m_ConsoleLevel <= eLogLevel::Error)
		{
			const std::string criticalString{ "[critical] " + formattedMessage + "\n" };
			Th::EventMessenger::GetInstance().EvokeMessenger("ConsoleMessage", criticalString);
		}
#elif PLATFORM_PS4
		if (m_ConsoleLevel <= eLogLevel::Error)
		{
			const std::string formattedMessage{ "[critical]" + message + "\n"};
			printf(formattedMessage.c_str(), std::forward<Args>(arguments)...);
		}

#endif
	}


#ifdef PLATFORM_WIN64
	template <typename ... Args>
	std::string MessageLogger::FormatSpdLogMessage(const std::string& message, Args&&... arguments)
	{
		return std::string{ fmt::sprintf(message,std::forward<Args>(arguments)...) };
	}
#endif
}

#define LOGDEBUG Th::MessageLogger::GetInstance().LogDebug
#define LOGINFO Th::MessageLogger::GetInstance().LogInfo
#define LOGWARNING Th::MessageLogger::GetInstance().LogWarning
#define LOGERROR Th::MessageLogger::GetInstance().LogError
#define LOGCRITICAL Th::MessageLogger::GetInstance().LogCritical

#if PLATFORM_PS4
#pragma GCC diagnostic pop
#endif