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

	config = (ConfigAction*) malloc(sizeof(ConfigAction));
	config->apply = false;
	config->load = false;
	config->save = false;

	drivetrain = (DriveTrainAction*) malloc(sizeof(DriveTrainAction));
	drivetrain->rate.drive_control = true;
	drivetrain->rate.turn_control = true;
	drivetrain->rate.desiredDriveRate = 0.0;
	drivetrain->rate.desiredTurnRate = 0.0;
	drivetrain->position.drive_control = false;
	drivetrain->position.turn_control = false;
	drivetrain->position.desiredRelativeDrivePosition = 0.0;
	drivetrain->position.desiredRelativeTurnPosition = 0.0;
	drivetrain->previousDriveOperationComplete = true;
	drivetrain->previousTurnOperationComplete = true;
	drivetrain->setDriveOperation = false;
	drivetrain->setTurnOperation = false;
	drivetrain->overrideOperationChecks = false;
	drivetrain->raw.leftDutyCycle = 0.0;
	//	drivetrain->raw.leftBrakingDutyCycle = 0.0;
	drivetrain->raw.rightDutyCycle = 0.0;
	//	drivetrain->raw.rightBrakingDutyCycle = 0.0;

	shifter = (ShifterAction*) malloc(sizeof(ShifterAction));
	shifter->force = false;
	shifter->gear = ACTION::GEARBOX::HIGH_GEAR;

	launcher = (LauncherAction*) malloc(sizeof(LauncherAction));
	launcher->state = ACTION::LAUNCHER::RUNNING;
	launcher->atSpeed = false;
	launcher->speed = 2000;
	launcher->ballLaunchCounter = 0;
	launcher->topTrajectory = false;

	wedge = (BPDAction*) malloc(sizeof(BPDAction));
	wedge->state = ACTION::WEDGE::IDLE;
	wedge->completion_status = ACTION::UNSET;

	ballfeed = (BallFeedAction*) malloc(sizeof(BallFeedAction));
	ballfeed->sweepArmOut = false;

	auton = (AutonomousAction *) malloc(sizeof(AutonomousAction));
	auton->state = ACTION::AUTONOMOUS::TELEOP;
	auton->completion_status = ACTION::UNSET;

	imu = (IMU_Data *) malloc(sizeof(IMU_Data));
	imu->accel_x = imu->accel_y = imu->accel_z = 0;
	imu->gyro_x = imu->gyro_y = imu->gyro_z = 0;
	imu->pitch = imu->roll = imu->yaw = 0.0;

	cam = (camera *) malloc(sizeof(camera));
	cam->key.blue = cam->key.red = cam->key.higher = 0;
	cam->align.arbitraryOffsetFromUDP = 0;
	cam->align.status = ACTION::CAMERA::NO_TARGET;
}

ActionData::~ActionData()
{
	free(config);
	free(drivetrain);
	free(launcher);
	free(shifter);
	free(wedge);
	free(ballfeed);
}
