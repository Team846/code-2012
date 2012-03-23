#ifndef SYNC_PROCESS_H_
#define SYNC_PROCESS_H_

#include "Process.h"
#include "Profiler.h"

/*!
 * Implements a synchronous blocking process
 * @author Robert Ying
 */
class SyncProcess: Process
{
public:
	/*!
	 * Constructs a SyncProcess
	 * @param name Name of process
	 */
	SyncProcess(const char * name)
	{
		if (name)
		{
			m_name = name;
		}
		else
		{
			m_name = "UnknownTask";
		}
	}

	/*!
	 * Destructs a SyncProcess
	 */
	virtual ~SyncProcess()
	{

	}

	/*!
	 * Called in the beginning to initialize values
	 * Should be called only once
	 */
	virtual void init()
	{

	}

	/*!
	 * Called at the end to deallocate values
	 * Should be called in the destructor
	 */
	virtual void deinit()
	{

	}

	/*!
	 * Called each cycle to run the work function
	 */
	virtual void startNewProcessCycle()
	{
		ProfiledSection ps("Sync " + m_name);
		work();
	}

	/*!
	 * Returns the name of this SyncProcess
	 * @return name
	 */
	std::string getName()
	{
		return m_name;
	}

private:
	std::string m_name;
};
#endif
