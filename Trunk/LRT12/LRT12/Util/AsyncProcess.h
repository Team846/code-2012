#ifndef ASYNC_PROCESS_H_
#define ASYNC_PROCESS_H_

#include "Process.h"
#include "WPILib.h"

/*!
 * Implements an asynchronous process
 * @author Robert Ying
 */
class AsyncProcess: public Process
{
public:
	/*!
	 * Constructs an AsyncProcess
	 * @param name Name of process
	 * @param priority Priority of task
	 */
	AsyncProcess(const char * name, uint32_t priority = Task::kDefaultPriority);

	/*!
	 * Destructs an AsyncProcess
	 */
	~AsyncProcess();

	/*!
	 * Called in the beginning to initialize values
	 * Should be called only once
	 */
	virtual void init();

	/*!
	 * Called at the end to deallocate values
	 * Should be called in the destructor
	 */
	virtual void deinit();

	/*!
	 * Called each cycle to synchronize tasks via semaphore
	 */
	virtual void startNewProcessCycle();

	/*!
	 * Returns the name of this AsyncProcess
	 * @return name
	 */
	std::string getName();

private:
	std::string m_name;
	SEM_ID m_task_sem;
	Task * m_task;
	bool m_is_running;

	/*!
	 * Entry point for task
	 * @param ptr pointer to object
	 */
	static void taskEntryPoint(void * ptr);

	/*!
	 * Actual task
	 */
	void task();
};

#endif
