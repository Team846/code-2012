#ifndef DRIVETRAIN_ACTION_H_
#define DRIVETRAIN_ACTION_H_
#include "ActionData.h"

struct DriveTrainAction
{
	struct
	{
		bool drive_control;
		bool turn_control;

		float desiredDriveRate;
		float desiredTurnRate;
	} rate;

	struct
	{
		bool drive_control;
		bool turn_control;

		float desiredRelativeDrivePosition;
		float desiredRelativeTurnPosition;
	} position;

	bool previousDriveOperationComplete;
	bool previousTurnOperationComplete;

	bool setDriveOperation;
	bool setTurnOperation;

	int synchronizedCyclesRemaining;
};

#endif
