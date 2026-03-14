#include "Logger.h"
namespace ALogger
{
	std::string Logger::logLevelToString(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::DEBUG:
			return "DEBUG";
		case LogLevel::INFO:
			return "INFO";
		case LogLevel::WARNING:
			return "WARNING";
		case LogLevel::ERROR:
			return "ERROR";
		case LogLevel::CRITICAL:
			return "CRITICAL";
		default:
			return "UNKNOWN";
		}
	}

	void Logger::SetLogState(bool state)
	{
		logOn = state;
	}
	void Logger::SetMinLogLevel(LogLevel level)
	{
		minLogLevel = level;
	}
} // namespace ALogger