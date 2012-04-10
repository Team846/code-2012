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
#if USE_NOTIFIER
	loopSynchronizer = new Notifier(&releaseLoop, this);
#endif
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
#if USE_NOTIFIER
	loopSynchronizer->Stop();
	delete loopSynchronizer;
	loopSynchronizer = NULL;
	int error = semDelete(loopSemaphore);
	if (error)
	{
		printf("SemDelete Error=%d\r\n", error);
	}
	else
	{
		loopSemaphore = NULL;
	}
#endif

	if (AsyncCANJaguar::jaguar_list_)
	{
		for (AsyncCANJaguar* j = AsyncCANJaguar::jaguar_list_; j != NULL; j
				= j->next_jaguar_)
		{
			j->deinit();
		}
	}

	AutonomousFunctions::getInstance()->deinit();
	Pneumatics::getInstance()->deinit();
	Log::getInstance()->deinit();

#if FANCY_SHIT_ENABLED
	m_imu->deinit();
	delete m_imu;
#endif

	AsyncPrinter::Quit();
	
	printf("Deleting LRTRobotBase\n\n"); //should be our last access to the program.
}

/**
 * Used to continuously call the MainLoop method while printing diagnostics.
 */
void LRTRobotBase::StartCompetition()
{
	//Diagnostic: Print the task name.
	printf("vxWorks task: %s\n", m_task->GetName());

	GetWatchdog().SetEnabled(false);

	// first and one-time initialization
	RobotInit();

#if USE_NOTIFIER
	AsyncPrinter::Printf("starting synchronizer\r\n");
	loopSynchronizer->StartPeriodic(1.0 / RobotConfig::LOOP_RATE); //arg is period in seconds
#endif

	AsyncPrinter::Printf("Starting Pneumatics\r\n");
	Pneumatics::getInstance()->init();

	AsyncPrinter::Printf("Starting Logger\r\n");
	Log::getInstance()->init();

	AsyncPrinter::Printf("Starting Autonomous Functions\r\n");
	AutonomousFunctions::getInstance()->init();

	AsyncPrinter::Printf("Starting Jaguar tasks\r\n");

	if (AsyncCANJaguar::jaguar_list_)
	{
		for (AsyncCANJaguar* j = AsyncCANJaguar::jaguar_list_; j != NULL; j
				= j->next_jaguar_)
		{
			j->init();
		}
	}

#if FANCY_SHIT_ENABLED
	m_imu->init();
#endif

	AsyncPrinter::Printf("Starting Profiler\r\n");
	Profiler& profiler = Profiler::GetInstance();
	// loop until we are quitting -- must be set by the destructor of the derived class.

#if not USE_NOTIFIER
	double last = GetFPGATime() * 1.0e-6; //ms
#endif

	while (!quitting_)
	{
		// block the loop and allow other tasks to run until the notifier
		// releases our semaphore
#if USE_NOTIFIER
		if (loopSemaphore)
		{
			semTake(loopSemaphore, WAIT_FOREVER);
		}
#else
		last = GetFPGATime() * 1.0e-6;
#endif
		cycleCount++;
		if (quitting_)
		{
			break;
		}

		profiler.startNewProcessCycle();

#if FANCY_SHIT_ENABLED
		if (cycleCount % 2 == 0)
		{
			m_imu->startNewProcessCycle();
		}
#endif

		AutonomousFunctions::getInstance()->startNewProcessCycle();

		{
			ProfiledSection ps("Main Loop");
			MainLoop();
		}

		// release jaggie semaphores
		// NB: This loop must be quit *before* the Jaguars are deleted!
		if (AsyncCANJaguar::jaguar_list_)
		{
			for (AsyncCANJaguar* j = AsyncCANJaguar::jaguar_list_; j != NULL; j
					= j->next_jaguar_)
			{
				j->startNewProcessCycle();
			}
		}

		Pneumatics::getInstance()->startNewProcessCycle();

#if LOGGING_ENABLED
		if (cycleCount % 10 == 0)
		{
			Log::getInstance()->startNewProcessCycle();
		}
#endif

#if not USE_NOTIFIER
		double time_left_s =
		(20.0 * 1.0e-3 - ((GetFPGATime() * 1.0e-6) - last));
		if (time_left_s > 0.0)
		Wait(time_left_s);
		else
		AsyncPrinter::Printf("%.02f overflow\r\n", time_left_s);
#endif
	}
}

//called by interupt on timer. This structure with the semaphores is to avoid the restrictions of running an ISR.
void LRTRobotBase::releaseLoop(void* param)
{
	semGive(((LRTRobotBase*) param)->loopSemaphore);
}
