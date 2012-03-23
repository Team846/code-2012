#ifndef PROCESS_H_
#define PROCESS_H_

#include <string>

/*!
 * Virtual class that defines a process to be carried out
 * Should never be used directly.
 * @author Robert Ying
 */
class Process
{
public:
	/*!
	 * Called in the beginning to initialize values
	 * Should be called only once
	 */
	virtual void init() = 0;

	/*!
	 * Called at the end to deallocate values
	 * Should be called in the destructor
	 */
	virtual void deinit() = 0;

	/*!
	 * Called each cycle to start processing
	 */
	virtual void startNewProcessCycle() = 0;

	/*!
	 * Overloaded method for conducting the work
	 */
	virtual void work() = 0;

	/*!
	 * Returns the name of this process
	 * @return name
	 */
	virtual std::string getName() = 0;

	/*!
	 * Destructor
	 */
	virtual ~Process()
	{

	}
};

#endif
