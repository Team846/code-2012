#ifndef LRT_ROBOT_BASE_H_
#define LRT_ROBOT_BASE_H_

#include "flags.h"
#include "Util\Util.h"
#include "Util\Profiler.h"
#include "Util\AsyncPrinter.h"
#include <semLib.h>
#include <WPILib.h>
#include <vxWorks.h>

/**
 * IterativeRobot implements a specific type of Robot Program framework, extending the RobotBase class.
 *
 * The IterativeRobot class is intended to be subclassed by a user creating a robot program.
 *
 * This class is intended to implement the "old style" default code, by providing
 * the following functions which are called by the main loop, StartCompetition(), at the
 * appropriate times:
 *
 * RobotInit() -- provide for initialization at robot power-on
 *
 * Init() functions -- each of the following functions is called once when the
 *                     appropriate mode is entered:
 *  - DisabledInit()   -- called only when first disabled
 *  - AutonomousInit() -- called each and every time autonomous is entered from another mode
 *  - TeleopInit()     -- called each and every time teleop is entered from another mode
 *
 * Periodic() functions -- each of these functions is called iteratively
 *                         whenever a new packet is available
 *   - DisabledPeriodic()
 *   - AutonomousPeriodic()
 *   - TeleopPeriodic()
 *
 */

enum GameState
{
	DISABLED, AUTONOMOUS, TELEOPERATED
};

class IMU;

class LRTRobotBase: public RobotBase
{
public:
	virtual void StartCompetition();

	virtual void RobotInit() = 0;
	virtual void MainLoop() = 0;

protected:
	LRTRobotBase();
	virtual ~LRTRobotBase();
	bool quitting_; //use this to kill the main loop for orderly shutdown.

	int cycleCount;
	int packetsMissedInLifetime;
	//    double minCycleTime, maxCycleTime;

private:
	SEM_ID loopSemaphore;
	Notifier * loopSynchronizer;
	static void releaseLoop(void* param);

#if FANCY_SHIT_ENABLED
	IMU *m_imu;
#endif
};

#endif
