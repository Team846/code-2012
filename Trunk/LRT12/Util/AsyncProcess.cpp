#include "AsyncProcess.h"
#include "Profiler.h"

AsyncProcess::AsyncProcess(const char * name, uint32_t priority)
{
	if (name)
	{
		m_name = name;
	}
	else
	{
		m_name = "UnknownTask";
	}
	m_task = new Task(name, (FUNCPTR) AsyncProcess::taskEntryPoint, priority);
	m_task_sem = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
	m_is_running = false;
}

void AsyncProcess::init()
{
	if (!m_is_running)
	{
		m_is_running = true;
		m_task->Start(reinterpret_cast<UINT32> (this));
	}
}

void AsyncProcess::deinit()
{
	if (m_is_running)
	{
		m_is_running = false;
		UINT32 task_id = m_task->GetID();
		m_task->Stop();
		printf("Task 0x%x killed for %s\r\n", task_id, m_name.c_str());
	}
}

AsyncProcess::~AsyncProcess()
{
	deinit();
	delete m_task;
	m_task = NULL;

	int error = semDelete(m_task_sem);
	if (error)
	{
		printf("SemDelete Error=%d\r\n", error);
	}
	else
	{
		m_task_sem = NULL;
	}
}

void AsyncProcess::startNewProcessCycle()
{
	if (m_task_sem)
	{
		semGive(m_task_sem);
	}
}

void AsyncProcess::taskEntryPoint(void * ptr)
{
	AsyncProcess * a = reinterpret_cast<AsyncProcess*> (ptr);
	a->task();
}

void AsyncProcess::task()
{
	while (m_is_running)
	{
		if (m_task_sem)
		{
			semTake(m_task_sem, WAIT_FOREVER);
		}
		if (!m_is_running)
		{
			break;
		}
		{
			ProfiledSection ps("Async " + m_name);
			work();
		}
	}
}

std::string AsyncProcess::getName()
{
	return m_name;
}
