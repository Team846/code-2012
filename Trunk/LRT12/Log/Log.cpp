#include "Log.h"
#include "WPILib/SmartDashboard/SmartDashboard.h"

Log::Log()
{
}

Log::~Log()
{
	m_instance = NULL;
}

Log& Log::getInstance()
{
	if (m_instance == NULL)
	{
		m_instance = new Log();
	}
	return *m_instance;
}

void Log::logAll()
{
	for (uint32_t i = 0; i < m_loggables.size(); i++)
	{
		Loggable* ptr = m_loggables[i];

		if (ptr->shouldLog())
		{
			ptr->log();
		}
	}
}

void Log::registerLoggable(Loggable * loggable)
{
	m_loggables.push_back(loggable);
}
