#ifndef LRT_ROBOT_12_H_
#define LRT_ROBOT_12_H_

#include "flags.h"

#include "wdLib.h"
#include "sysLib.h"

#include "LRTRobotBase.h"
#include "Brain/Brain.h"
#include "Components/Component.h"
#include "Config/Config.h"
#include "Config/RobotConfig.h"
#include "Log/Log.h"

#include "Compressor.h"

#include "Util/AsyncPrinter.h"
#include "Util/PrintInConstructor.h"
#include "Jaguar/AsyncCANJaguar.h"
#include "Jaguar/JaguarTester.h"

#if FANCY_SHIT_ENABLED
#include "Sensors/Trackers.h"
#endif

#if SKETCHY_SHIT_ENABLED
#include "Util/NetworkHelper.h"
#endif

class LRTRobot12: public LRTRobotBase
{
public:
	PrintInConstructor firstMember_;
	LRTRobot12();
	virtual ~LRTRobot12();

	virtual void RobotInit();
	virtual void MainLoop();

private:
	//	JaguarTester jagTest;
	Compressor *m_compressor;
	DigitalInput *m_pressureSwitch;
	Brain brain;
	ActionData *m_action;

#if FANCY_SHIT_ENABLED
	Trackers *m_trackers;
#endif
	
#if SKETCHY_SHIT_ENABLED
	NetworkHelper *m_networkhelper;
#endif

	PrintInConstructor dc_CANBus_;

	Config *config;

	DriverStation* ds;

	GameState prevState;
	GameState DetermineState();

	list<Component::ComponentWithData>* components;

	WDOG_ID mainLoopWatchDog;
	PrintInConstructor lastMember_; //trace constructor/destructor.
};

#endif
