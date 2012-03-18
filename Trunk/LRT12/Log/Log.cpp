#include "Log.h"
#include "Loggable.h"

Log* Log::m_instance = NULL;
std::vector<Loggable*> Log::m_loggables;

Log::Log()
{
	m_task = new Task("LogTask", (FUNCPTR) Log::taskEntryPoint,
			Task::kDefaultPriority);
	m_is_running = false;
	m_sem = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
}

Log::~Log()
{
	getInstance()->stopTask();
	delete m_task;
	m_instance = NULL;

	int error = semDelete(m_sem);
	if (error)
	{
		printf("SemDelete Error=%d\n", error);
	}
}

void Log::startTask()
{
	m_is_running = true;
	m_task->Start();
}

void Log::stopTask()
{
	if (m_is_running)
	{
		UINT32 task_id = m_task->GetID();
		m_task->Stop();
		printf("Task 0x%x killed for Log\r\n", task_id);
		m_is_running = false;
	}
}

void Log::task()
{
	while (m_is_running)
	{
		semTake(m_sem, WAIT_FOREVER);
		if (!m_is_running)
		{
			break;
		}
		getInstance()->logAll();
	}
}

void Log::releaseSemaphore()
{
	semGive(m_sem);
}

int Log::taskEntryPoint()
{
	getInstance()->task();
	return 0;
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
//			ptr->log();
		}
	}
}

void Log::registerLoggable(Loggable * loggable)
{
	m_loggables.push_back(loggable);
}
