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

	DriveTrainAction()
	{
		previousDriveOperationComplete = true;
		previousTurnOperationComplete = true;
		setDriveOperation = false;
		setTurnOperation = false;
		overrideOperationChecks = false;
		synchronizedCyclesRemaining = 0;
		robotTranslateSpeed = 0;
		robotRotateSpeed = 0;

		rate.drive_control = true;
		rate.turn_control = true;
		rate.desiredDriveRate = 0;
		rate.desiredTurnRate = 0;

		memset(&position, 0, sizeof(position));
		memset(&rate, 0, sizeof(rate));
		memset(&raw, 0, sizeof(raw));
	}
};

namespace ACTION
{
namespace DRIVETRAIN
{
const static double UNSET_POS = 6564E16;
}
}

#endif
