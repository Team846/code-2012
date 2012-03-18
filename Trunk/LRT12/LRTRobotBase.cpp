#include "LRTRobotBase.h"
#include "LRTRobot12.h"
#include "Jaguar/AsyncCANJaguar.h"
#include "Components/Pneumatic/Pneumatics.h"
#include "Brain/AutonomousFunctions.h"
#include "Config/RobotConfig.h"
#include "Utility.h"
#include "sysLib.h"
#include "Sensors/IMU.h"

/**
 * Constructor for RobotIterativeBase. Initializes member variables.
 */
LRTRobotBase::LRTRobotBase()
{
	loopSynchronizer = new Notifier(&releaseLoop, this);
	quitting_ = false;
	cycleCount = 0;

	printf("Creating LRTRobotbase\n");
	loopSemaphore = semBCreate(SEM_Q_PRIORITY, SEM_FULL);

#if FANCY_SHIT_ENABLED
	m_imu = new IMU();
#endif
}

/**
 * Free the resources for a RobotIterativeBase class.
 */
LRTRobotBase::~LRTRobotBase()
{
	loopSynchronizer->Stop();
	delete loopSynchronizer;

	if (AsyncCANJaguar::jaguar_list_)
	{
		for (AsyncCANJaguar* j = AsyncCANJaguar::jaguar_list_; j != NULL; j
				= j->next_jaguar_)
		{
			j->StopBackgroundTask();
		}
	}

	AutonomousFunctions::getInstance()->stopBackgroundTask();
	Pneumatics::getInstance()->stopBackgroundTask();
	Log::getInstance()->stopTask();

#if FANCY_SHIT_ENABLED
	m_imu->stopTask();
	delete m_imu;
#endif

	printf("Deleting LRTRobotBase\n\n"); //should be our last access to the program.
	AsyncPrinter::Quit();
}

#define USE_NOTIFIER 0

/**
 * Used to continuously call the MainLoop method while printing diagnostics.
 */
void LRTRobotBase::StartCompetition()
{
	//Diagnostic: Print the task name.
	//m_teleop_task is available only after robot is initialized -dg
	printf("vxWorks task: %s\n", m_task->GetName());

	GetWatchdog().SetEnabled(true);

	// first and one-time initialization
	RobotInit();

	AsyncPrinter::Printf("starting synchronizer\r\n");

#if USE_NOTIFIER
	loopSynchronizer->StartPeriodic(1.0 / RobotConfig::LOOP_RATE); //arg is period in seconds
#endif

	AsyncPrinter::Printf("Starting Pneumatics\r\n");
	Pneumatics::getInstance()->startBackgroundTask();
	Log::getInstance()->startTask();
	AutonomousFunctions::getInstance()->startBackgroundTask();

#if FANCY_SHIT_ENABLED
	m_imu->startTask();
#endif

	AsyncPrinter::Printf("Starting Profiler\r\n");
	Profiler& profiler = Profiler::GetInstance();
	// loop until we are quitting -- must be set by the destructor of the derived class.

	double last = GetFPGATime() * 1.0e-6; //ms

	while (!quitting_)
	{
		// block the loop and allow other tasks to run until the notifier
		// releases our semaphore
#if USE_NOTIFIER
		semTake(loopSemaphore, WAIT_FOREVER);
#else
		last = GetFPGATime() * 1.0e-6;
#endif
		cycleCount++;
		if (quitting_)
		{
			break;
		}

#if FANCY_SHIT_ENABLED
		if (cycleCount % 2 == 0)
		{
			m_imu->releaseSemaphore();
		}
#endif

		AutonomousFunctions::getInstance()->releaseSemaphore();

		profiler.StartNewCycle();

		{
			ProfiledSection ps("Main Loop");
			GetWatchdog().Feed();
			MainLoop();
		}

		// release jaggie semaphores
		// NB: This loop must be quit *before* the Jaguars are deleted!
		if (AsyncCANJaguar::jaguar_list_)
		{
			for (AsyncCANJaguar* j = AsyncCANJaguar::jaguar_list_; j != NULL; j
					= j->next_jaguar_)
			{
				j->ReleaseCommSemaphore();
			}
		}

		Pneumatics::getInstance()->releaseSemaphore();

		if (cycleCount % 500 == 0)
		{
			printf("Time: %.4fms\n", GetFPGATime() * 1.0e-6);
		}

#if LOGGING_ENABLED
		if (cycleCount % 10 == 0)
		{
			Log::getInstance()->releaseSemaphore();
		}
#endif
		double time_left_s =
				(20.0 * 1.0e-3 - ((GetFPGATime() * 1.0e-6) - last));
		if (time_left_s > 0.0)
			Wait(time_left_s);
		else
			AsyncPrinter::Printf("%.02f overflow\r\n", time_left_s);
	}
}

//called by interupt on timer. This structure with the semaphores is to avoid the restrictions of m_is_running an ISR.
void LRTRobotBase::releaseLoop(void* param)
{
	semGive(((LRTRobotBase*) param)->loopSemaphore);
}
