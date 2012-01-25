#include "ActionData.h"
#include "DriveTrainAction.h"
#include "ConfigAction.h"

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
}

ActionData::~ActionData()
{
	free(config);
	free(drivetrain);
}
