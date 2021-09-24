#pragma once

#include <fstream>
#include <mutex>


class Logger
{
public:
	Logger();
	virtual ~Logger();
	static Logger& instance();
	enum class Type
	{
		Info,
		Error
	};

	static void logError(const std::string& _error);
	static void logInfo(const std::string& _error);
	void log(const std::string& _error, Type _type);

	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;
	Logger(Logger&&) = delete;
	Logger& operator=(Logger&&) = delete;

private:
	std::ofstream m_file;
	std::mutex m_mu;
};

