#pragma once
#include "FileLogger.h"

namespace ALogger
{

	FileLogger::FileLogger()
	{
		logFile.open("log.txt", std::ios::out | std::ios::app);
		if (!logFile.is_open())
		{
			std::cerr << "Failed to open log file: log.txt" << std::endl;
		}
	}

	FileLogger::FileLogger(const std::filesystem::path &filePath)
	{
		logFile.open(filePath, std::ios::out | std::ios::trunc); // Open for writing in trunc mode
		if (!logFile.is_open())
		{
			std::cerr << "Failed to open log file: " << filePath << std::endl;
		}
	}

	FileLogger::~FileLogger()
	{
		if (logFile.is_open())
		{
			logFile.close();
		}
	}

	void FileLogger::Log(const std::string &message, ALogger::LogLevel level, const char *file, int line)
	{
		// std::lock_guard<std::mutex> guard(logMutex);
		if(!logOn)
			return;

		if(level < minLogLevel)
			return;

		if (!logFile.is_open())
		{
			std::cerr << "Attempt to log to an unopened file." << std::endl;
			return;
		}

		// Get current time
		auto now = std::chrono::system_clock::now();
		std::time_t now_time = std::chrono::system_clock::to_time_t(now);
		std::tm now_tm = *std::localtime(&now_time);

		// Convert log level to string
		const std::string levelStr = logLevelToString(level);

		// Format and write the log message
		logFile << "[" << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S") << "] "
				<< "[" << levelStr << "] ";

		if (file != nullptr)
		{
			logFile << "[" << file << ":" << line << "] ";
		}

		logFile << message << std::endl;
	}
	void FileLogger::Log(const std::string &message, ALogger::LogLevel level)
	{
		Log(message, level, nullptr, 0);
	}
}
