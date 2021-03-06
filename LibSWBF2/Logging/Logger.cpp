#include "stdafx.h"
#include "Logger.h"

namespace LibSWBF2::Logging
{
	std::unique_ptr<Logger> Logger::m_Instance = nullptr;

	Logger::Logger()
	{
		m_Writer.Open(LOG_FILE, true);
	}

	Logger::~Logger()
	{
		m_Writer.Close();
	}

	std::unique_ptr<Logger>& Logger::GetInstance()
	{
		if (m_Instance == nullptr)
		{
			m_Instance = std::make_unique<Logger>();
		}

		return m_Instance;
	}

	void Logger::SetLogCallback(const LogCallback Callback)
	{
		GetInstance()->m_OnLogCallback = Callback;
	}

	void Logger::SetLogfileLevel(const ELogType LogfileLevel)
	{
		GetInstance()->m_LogfileLevel = LogfileLevel;
	}
	
	void Logger::Log(const std::string &message, const ELogType level, const unsigned long line, const char* file)
	{
		if (message.length() > 0 && level >= m_LogfileLevel)
		{
			LoggerEntry entry(message.c_str(), level, line, file);
			m_Writer.WriteLine(entry.ToString());

			if (m_OnLogCallback != nullptr)
			{
				m_OnLogCallback(&entry);
			}
		}
	}
}