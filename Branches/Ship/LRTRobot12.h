#ifndef LRT_ROBOT_12_H_
#define LRT_ROBOT_12_H_

#include "LRTRobotBase.h"
#include "Brain/Brain.h"
#include "Components/Component.h"
#include "Config/Config.h"
#include "Config/RobotConfig.h"
#include "Log/Log.h"
#include "wdLib.h"
#include "sysLib.h"
#include "Compressor.h"
#include "Jaguar/JaguarTester.h"

#include "Util/AsyncPrinter.h"
#include "Util/PrintInConstructor.h"

#include "Jaguar/AsyncCANJaguar.h"

class LRTRobot12: public LRTRobotBase
{
public:
	PrintInConstructor firstMember_;
	LRTRobot12();
	virtual ~LRTRobot12();

	virtual void RobotInit();
	virtual void MainLoop();

private:
	JaguarTester jagTest;
	Compressor *m_compressor;
	DigitalInput *m_pressureSwitch;
	Brain brain;
	ActionData *m_action;

	PrintInConstructor dc_CANBus_;

	Config *config;

	DriverStation* ds;

	GameState prevState;
	GameState DetermineState();

	list<Component::ComponentWithData>* components;

	//    AnalogChannel armPot;
	WDOG_ID mainLoopWatchDog;
	PrintInConstructor lastMember_; //trace constructor/destructor.
};

#endif
