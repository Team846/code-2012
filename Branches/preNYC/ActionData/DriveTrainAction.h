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

		double desiredAbsoluteDrivePosition;
		double desiredAbsoluteTurnPosition;
		
		bool reset_translate_zero;
		bool reset_turn_zero;
		
		bool absoluteTurn;
		bool absoluteTranslate;
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
	
	double robotTranslateSpeed;
	double robotRotateSpeed;
};

namespace ACTION
{
	namespace DRIVETRAIN
	{
		const static double UNSET_POS = 6564E16;
	}
}

#endif
