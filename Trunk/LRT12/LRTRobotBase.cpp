#include "LRTRobotBase.h"
#include "Jaguar/AsyncCANJaguar.h"
#include "Utility.h"
#include "sysLib.h"

/**
 * Constructor for RobotIterativeBase. Initializes member variables.
 */
LRTRobotBase::LRTRobotBase()
{
	loopSynchronizer = new Notifier(&releaseLoop, this);
	jagUpdater = new Notifier(&updateJaguars, this);
	quitting_ = false;
	cycleCount = 0;

	printf("Creating LRTRobotbase\n");
	loopSemaphore = semBCreate(SEM_Q_PRIORITY, SEM_FULL);
}

/**
 * Free the resources for a RobotIterativeBase class.
 */
LRTRobotBase::~LRTRobotBase()
{
	loopSynchronizer->Stop();
	delete loopSynchronizer;

	jagUpdater->Stop();
	delete jagUpdater;

	for (AsyncCANJaguar* j = j->jaguar_list_; j != NULL; j = j->next_jaguar_)
	{
		j->StopBackgroundTask();
	}

	printf("Deleting LRTRobotBase\n\n"); //should be our last access to the program.
	AsyncPrinter::Quit();
}

/**
 * Used to continuously call the MainLoop method while printing diagnostics.
 */
void LRTRobotBase::StartCompetition()
{
	//Diagnostic: Print the task name.
	//m_teleop_task is available only after robot is initialized -dg
	printf("vxWorks task: %s\n", m_task->GetName());

	GetWatchdog().SetEnabled(false);

	// first and one-time initialization
	RobotInit();
	Profiler& profiler = Profiler::GetInstance();

	// must allow a negative value in case loop runs over 20ms
	//	INT32 sleepTime_us = 0;

	AsyncPrinter::Printf("starting synchronizer");
	loopSynchronizer->StartPeriodic(1.0 / 50.0); //arg is period in seconds
	jagUpdater->StartPeriodic(1.0 / 200.0);

	// loop until we are quitting -- must be set by the destructor of the derived class.

	while (!quitting_)
	{
		cycleCount++;
		semTake(loopSemaphore, WAIT_FOREVER);

		profiler.StartNewCycle();

		{
			ProfiledSection ps("Main Loop");
			MainLoop();
		}

		if (cycleCount % 100 == 0)
		{
			//          printf("Cycle count: %d\n", cycleCount);
			//			AsyncPrinter::Printf("Sleep time: %.2fms\n\n",
			//sleepTime_us		* 1.0e-3);
			printf("Time: %.4fms\n", GetFPGATime() * 1.0e-3);
			//          printf("Target Time: %.4fms\n", cycleExpire_us * 1.0e-3);
			//          fflush(stdout);
		}
	}
}

//called by interupt on timer. This structure with the semaphores is to avoid the restrictions of running an ISR.
void LRTRobotBase::releaseLoop(void* param)
{
	// RY: This was massively polluting the console
	//	AsyncPrinter::Printf("%d\n", GetFPGATime());
	semGive(((LRTRobotBase*) param)->loopSemaphore);
	Wait(0.01); //give the thread up to 1 ms to start
	//taskDelay(sysClkRateGet()/50/5);//check that this is at least 1 tick
	semTake(((LRTRobotBase*) param)->loopSemaphore, NO_WAIT);
}

void LRTRobotBase::updateJaguars(void* param)
{
	// release jaggie semaphores
	// NB: This loop must be quit *before* the Jaguars are deleted!
	for (AsyncCANJaguar* j = j->jaguar_list_; j != NULL; j = j->next_jaguar_)
	{
		j->BeginComm();
	}
}
