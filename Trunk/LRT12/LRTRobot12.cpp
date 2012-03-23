#include "LRTRobot12.h"
#include "Util/PrintInConstructor.h"
#include "ActionData/IMUData.h"
#include "Sensors/DriveEncoders.h"

#include <fstream>

LRTRobot12::LRTRobot12() :
			firstMember_(
					"\n\n\n---------------------------------------------------------\n"
						"Begin LRTRobot Constructor\n",
					"LRTRobot Destroyed\n\n"), dc_CANBus_("CANbus\n"),
			lastMember_("LRTRobot.LastMember\n") //trace constructor.
{

#if PRINT_TO_FILE_ON_STARTUP
	std::ofstream fout("start.txt", std::fstream::app);
	fout << "started\n";
	fout.close();
#else
	cout << "started" << endl;
#endif
	config = Config::GetInstance();
	prevState = DISABLED;
	ds = DriverStation::GetInstance();
	components = Component::CreateComponents();
	m_action = ActionData::GetInstance();

	m_compressor = new Compressor(
			RobotConfig::DIGITAL_IO::COMPRESSOR_PRESSURE_SENSOR_PIN,
			RobotConfig::RELAY_IO::COMPRESSOR_RELAY);
	m_compressor->Start();

	mainLoopWatchDog = wdCreate();

#if FANCY_SHIT_ENABLED
	m_trackers = new Trackers();
#endif

	printf("---- Robot Initialized ----\n\n");
}

LRTRobot12::~LRTRobot12()
{
	brain.deinit();

	m_compressor->Stop();
	delete m_compressor;

#if FANCY_SHIT_ENABLED
	delete m_trackers;
#endif

	printf("\n\nBegin Deleting LRTRobot12\n");

	// Kill the main loop, so we don't access deleted objects. -dg
	LRTRobotBase::quitting_ = true;
	printf("LRTRobot12 says to LRTRobotBase: \"Quit Main Loop please\"\n");
	Wait(0.100); //Wait for main loop to exec one last time and then exit.  Should take < 20ms.
}

void LRTRobot12::RobotInit()
{
	config->ConfigureAll();
	brain.init();
}

static int ExecutionNotify(...)
{
	printf("mainexec >20\r\n");
	return 0;
}

void LRTRobot12::MainLoop()
{
	// setup a watchdog to warn us if our loop takes too long
	// sysClkRateGet returns the number of ticks per cycle at the current clock rate.
	wdStart(mainLoopWatchDog, sysClkRateGet() / RobotConfig::LOOP_RATE,
			ExecutionNotify, 0);

	brain.startNewProcessCycle();

	GameState gameState = DetermineState();
	m_action->wasDisabled = (prevState == DISABLED);

	m_action->motorsEnabled = ds->GetDigitalIn(
			RobotConfig::DRIVER_STATION::MOTORS);

	//iterate though and output components
	for (list<Component::ComponentWithData>::iterator iter =
			components->begin(); iter != components->end(); iter++)
	{
		// if we are enabled or the Component does not require the enabled state
		if (IsEnabled() || !((*iter).second.RequiresEnabledState))
		{
			int DIO = (*iter).second.DS_DIOToDisableComponent;
			if (DIO == Component::ComponentData::NO_DS_DISABLE_DIO
					|| ds->GetDigitalIn(DIO))
			{
				ProfiledSection ps("Outputting " + (*iter).first->GetName());
				(*iter).first->Output();
			}
			else
			{
				ProfiledSection ps("Disabling " + (*iter).first->GetName());
				(*iter).first->Disable();
			}
		}
	}

	if (ds->GetDigitalIn(RobotConfig::DRIVER_STATION::COMPRESSOR))
	{
		m_compressor->Start();
	}
	else
	{
		m_compressor->Stop();
	}

	prevState = gameState;

	// if we finish in time, cancel the watchdog's error message
	wdCancel(mainLoopWatchDog);
}

GameState LRTRobot12::DetermineState()
{
	GameState state = TELEOPERATED;

	if (IsDisabled())
		state = DISABLED;
	else if (IsAutonomous())
		state = AUTONOMOUS;

	return state;
}

/*!
 * @brief FRC_UserProgram_StartupLibraryInit()
 *  is the entry point of the program, like main().
 *
 * The FRC_UserProgram_StartupLibraryInit() calls RobotBase::startRobotTask((FUNCPTR)FRC_userClassFactory)
 * which creates a task called "FRC_RobotTask"
 * that  ultimately calls FRC_userClassFactory() to create a new "LRTRobot12"
 * and then calls the virtual RobotBase::StartCompetition()
 * -> LRTRobotBase::StartCompetition();
 * This VxWorks task is named "FRC_RobotTask"
 * See WPILIB Robotbase.cpp
 * @author D.Giandomenico (description of WPLIB start code)
 * @author Brian Axelrod
 * @author Robert Ying
 * @author WPI
 */

//START_ROBOT_CLASS(LRTRobot12); //Expand the macro as below:

RobotBase *FRC_userClassFactory()
{
	return new LRTRobot12();
}
extern "C"
{
INT32 FRC_UserProgram_StartupLibraryInit()
{
	RobotBase::startRobotTask((FUNCPTR) FRC_userClassFactory);
	return 0;
}
}

