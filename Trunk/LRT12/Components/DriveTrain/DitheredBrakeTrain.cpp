#include "DitheredBrakeTrain.h"

DitheredBrakeTrain::DitheredBrakeTrain()
// TODO fix initialization
:
	leftEncoder(DriveEncoders::GetInstance().getLeftEncoder()),
			rightEncoder(DriveEncoders::GetInstance().getRightEncoder())
{
	Configure();
}

DitheredBrakeTrain::~DitheredBrakeTrain()
{
	// nothing
}

void DitheredBrakeTrain::Configure()
{
}

// linearizes speed controller - motor response
ESCCommand DitheredBrakeTrain::CalculateBrakeAndDutyCycle(float desired_speed,
		float current_speed)
{
	ESCCommand command;

	command.dutyCycle = 0.0;
	command.brakingDutyCycle = 0.0;

	if (current_speed < 0)
	{
		command = CalculateBrakeAndDutyCycle(-desired_speed, -current_speed);
		command.dutyCycle = -command.dutyCycle;
		return command;
	}

	// speed >= 0 at this point
	if (desired_speed >= current_speed) // trying to go faster
	{
		command.dutyCycle = desired_speed;
		command.brakingDutyCycle = 0.0;
	}
	// trying to slow down
	else
	{
		float error = desired_speed - current_speed; // error always <= 0

		if (desired_speed >= 0) // braking is based on speed alone; reverse power unnecessary
		{
			command.dutyCycle = 0.0; // must set 0 to brake

			if (current_speed > -error)
				command.brakingDutyCycle = -error / current_speed; // speed always > 0
			else
				command.brakingDutyCycle = 1.0;
		}
		else // input < 0; braking with reverse power
		{
			command.brakingDutyCycle = 0.0; // not braking
			command.dutyCycle = error / (1.0 + current_speed); // dutyCycle <= 0 because error <= 0
		}
	}

	return command;
}

DriveCommand DitheredBrakeTrain::Drive(float forwardInput, float turnInput)
{
	DriveCommand drive;

	float leftInput = forwardInput - turnInput;
	float rightInput = forwardInput + turnInput;

	if (Util::Abs<float>(leftInput) > 1.0)
	{
		// TODO must decide whether forward or turn takes precedence
		leftInput = Util::Sign<float>(leftInput) * 1.0;
	}

	if (Util::Abs<float>(rightInput) > 1.0)
	{
		// TODO must decide whether forward or turn takes precedence
		rightInput = Util::Sign<float>(rightInput) * 1.0;
	}

	float leftSpeed = DriveEncoders::GetInstance().getNormalizedMotorSpeed(
			leftEncoder);
	float rightSpeed = DriveEncoders::GetInstance().getNormalizedMotorSpeed(
			rightEncoder);

	drive.leftCommand = CalculateBrakeAndDutyCycle(leftInput, leftSpeed);
	drive.rightCommand = CalculateBrakeAndDutyCycle(rightInput, rightSpeed);

	return drive;
}
