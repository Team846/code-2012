#ifndef LRT_ASYNCPRINT_H_
#define LRT_ASYNCPRINT_H_

#include <queue>
#include <xstring>
#include <WPILib.h>

using std::string;

/*!
 * @brief Provides an asynchronous equivilent of printf. 
 */
class AsyncPrinter
{
public:
	virtual ~AsyncPrinter(); //why is this virtual? -dg
	static AsyncPrinter& Instance();

	/*!
	 * @brief Asynchronous alternative to Printf
	 */
	static int Printf(const char* format, ...);
	static void Quit();
	static bool QueueEmpty()
	{
		return Instance().queue_.empty();
	}

protected:
	AsyncPrinter();

private:
	static int PrinterTaskWrapper(); //match (FUNCPTR):  int *FUNCPTR(...)
	int PrinterTask();

	bool quitting_; //if true, then we are exiting and closing down the print queue.
	bool running_; //true if print task is m_is_running.

	SEM_ID semaphore_;
	int queue_bytes_;

	Task * printerTask; //T vsWorks background task that prints the buffered output.
	queue<string> queue_;

	const static int kMaxBuffer_ = 4096;DISALLOW_COPY_AND_ASSIGN(AsyncPrinter);
};

#endif