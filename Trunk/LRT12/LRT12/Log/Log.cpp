#include "Log.h"
#include "Loggable.h"

Log* Log::m_instance = NULL;
std::vector<Loggable*> Log::m_loggables;

Log::Log() :
	AsyncProcess("LogTask")
{
}

Log::~Log()
{
	getInstance()->deinit();
	m_instance = NULL;
}

void Log::work()
{
	getInstance()->logAll();
}

Log * Log::getInstance()
{
	if (m_instance == NULL)
	{
		m_instance = new Log();
	}
	return m_instance;
}

void Log::logAll()
{
	for (uint32_t i = 0; i < m_loggables.size(); i++)
	{
		Loggable* ptr = m_loggables.at(i);

		if (ptr->shouldLog())
		{
#if LOGGING_ENABLED
			ptr->log();
#endif
		}
	}
}

void Log::registerLoggable(Loggable * loggable)
{
	m_loggables.push_back(loggable);
}
