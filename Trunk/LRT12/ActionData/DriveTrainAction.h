#ifndef DRIVETRAIN_ACTION_H_
#define DRIVETRAIN_ACTION_H_
#include "ActionData.h"

struct DriveTrainAction
{
	struct
	{
		bool drive_control;
		bool turn_control;

		double desiredDriveRate;
		double desiredTurnRate;
	} rate;

	struct
	{
		bool drive_control;
		bool turn_control;

		double desiredRelativeDrivePosition;
		double desiredRelativeTurnPosition;
		
		bool reset_translate_zero;
	} position;

	struct
	{
		double leftDutyCycle;
//		double leftBrakingDutyCycle;
		double rightDutyCycle;
//		double rightBrakingDutyCycle;
	} raw;

	bool previousDriveOperationComplete;
	bool previousTurnOperationComplete;

	bool setDriveOperation;
	bool setTurnOperation;

	bool overrideOperationChecks;

	int synchronizedCyclesRemaining;
};

#endif
