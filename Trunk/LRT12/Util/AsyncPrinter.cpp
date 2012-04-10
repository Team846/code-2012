#include "AsyncPrinter.h"

#define PRINT 1

AsyncPrinter& AsyncPrinter::Instance()
{
	static AsyncPrinter printer;
	return printer;
}

AsyncPrinter::AsyncPrinter()
{
	printerTask = new Task("LRT-AsynchronousPrinter",
			(FUNCPTR) AsyncPrinter::PrinterTaskWrapper);
	quitting_ = false;
	running_ = false;
	semaphore_ = semMCreate(
			SEM_Q_PRIORITY | SEM_DELETE_SAFE | SEM_INVERSION_SAFE);
	queue_bytes_ = 0;

	printerTask->Start();
}

AsyncPrinter::~AsyncPrinter()
{
	// Printer is static, so we can't delete it...
	// task killed upon quitting
}

int AsyncPrinter::Printf(const char* format, ...)
{
#if !PRINT
	return 0; // # of bytes printed
#endif
	char buffer[256];

	AsyncPrinter& me = Instance();
	if (me.quitting_) //the program is quitting. Abort.
		return 0; // # of bytes printed

	va_list args;
	va_start(args, format);

	int n_bytes = vsprintf(buffer, format, args);
	va_end(args);

	if (n_bytes >= 0)
	{
		Synchronized s(me.semaphore_);
		if (me.quitting_) //the program is quitting & waiting for us.  Stop.
			return 0; // # of bytes printed.

		string str(buffer);

		me.queue_.push(str);
		me.queue_bytes_ += n_bytes;

		if (me.queue_bytes_ >= kMaxBuffer_)
		{
			while (!me.queue_.empty())
				me.queue_.pop();

			string overflow("(AsyncPrinter Buffer Overflow)\n");

			me.queue_.push(overflow);
			me.queue_bytes_ = overflow.length();
		}
	}
	return n_bytes;
}

//May be called externally to stop printing.
void AsyncPrinter::Quit()
{
	Instance().quitting_ = true;
	
//	while(Instance().running_) ;
	Wait(0.010); // wait for task to finish
	
	// kill if the task is still running
	if (Instance().running_)
	{
		Instance().printerTask->Stop();
		printf("[WARNING] AsyncPrinter task killed.\n");
	}
	
	semDelete(Instance().semaphore_);

	//For some reason, this destructor isn't getting called when the robot task is killed -dg
	// I never see this line printed.  Hence the call to Aysync...Quit() in the rebot dtor
	printf("Async Printer deleted\n\n");
}

int AsyncPrinter::PrinterTaskWrapper()
{
#if !PRINT
	Instance().running_ = false;
	return 0; //printer task dies.
#endif
	Instance().running_ = true;
	int status = Instance().PrinterTask();
	printf("AsyncPrinter: Stopping Async Printing\n");
	Instance().running_ = false;
	
	return status;
}

int AsyncPrinter::PrinterTask()
{
	while (!quitting_)
	{
		while (!quitting_ && !queue_.empty())
		{
			string str;
			{
				//Critical block
				Synchronized s(semaphore_);
				if (quitting_) //the program is quitting & waiting for us.  Stop.
					return 0;
				str = queue_.front();
				queue_.pop();
				queue_bytes_ -= str.length();
			}

			printf(str.c_str());
			Wait(0.001); // allow other tasks to run
		}

		Wait(0.002); // allow other tasks to run
	}
	return 0; //no error
}
