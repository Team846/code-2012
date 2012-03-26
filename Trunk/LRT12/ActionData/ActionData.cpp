#include "ActionData.h"
#include "AutonomousAction.h"
#include "DriveTrainAction.h"
#include "LauncherAction.h"
#include "ShifterAction.h"
#include "BPDAction.h"
#include "BallFeedAction.h"
#include "ConfigAction.h"
#include "CameraData.h"
#include "IMUData.h"

ActionData* ActionData::instance = NULL;

ActionData* ActionData::GetInstance()
{
	if (instance == NULL)
		instance = new ActionData();
	return instance;
}

ActionData::ActionData()
{
	// used to abort movements
	master.abort = false;

	wasDisabled = true;

	config = new ConfigAction;
	drivetrain = new DriveTrainAction;
	shifter = new ShifterAction;
	launcher = new LauncherAction;
	wedge = new BPDAction;
	ballfeed = new BallFeedAction;
	auton = new AutonomousAction;
	imu = new IMU_Data;
	cam = new CameraData;

	motorsEnabled = true;
}

ActionData::~ActionData()
{
	delete config;
	delete drivetrain;
	delete shifter;
	delete launcher;
	delete wedge;
	delete ballfeed;
	delete auton;
	delete imu;
	delete cam;
}
