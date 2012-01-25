#include "Drivetrain.h"
#include "../Util/AsynchronousPrinter.h"
#include "../ActionData/DriveTrainAction.h"

Drivetrain::Drivetrain() :
	Component(), m_name("Drive Train"),
			m_encoders(DriveEncoders::GetInstance())
{
	m_esc_left
			= new Esc(RobotConfig::CAN::DRIVE_LEFT_A,
					RobotConfig::CAN::DRIVE_LEFT_B,
					m_encoders.GetLeftEncoder(), "left");
	m_esc_right = new Esc(RobotConfig::CAN::DRIVE_RIGHT_A,
			RobotConfig::CAN::DRIVE_RIGHT_B, m_encoders.GetRightEncoder(),
			"right");
}

Drivetrain::~Drivetrain()
{
	delete m_esc_left;
	delete m_esc_right;
}

void Drivetrain::Configure()
{

}

std::string Drivetrain::GetName()
{
	return m_name;
}

void Drivetrain::Output()
{
	// only try new operation if previous one is complete, else discard
	if (m_drive_control.driveOperationComplete())
	{
		if (action->drivetrain->rate.drive_control)
		{
			m_drive_control.setDriveControl(ClosedLoopDrivetrain::CL_RATE);
			m_drive_control.setDriveRate(
					action->drivetrain->rate.desiredDriveRate);
		}
		else if (action->drivetrain->position.drive_control)
		{
			m_drive_control.setDriveControl(ClosedLoopDrivetrain::CL_POSITION);
			m_drive_control.setRelativeDrivePosition(
					action->drivetrain->position.desiredRelativeDrivePosition);
			// command has been set, so now zero the relative pos
			// this serves as a crude one-command queue
			action->drivetrain->position.desiredRelativeDrivePosition = 0;
		}
		else
		{
			m_drive_control.setDriveControl(ClosedLoopDrivetrain::CL_DISABLED);
			m_drive_control.setRawDriveDutyCycle(
					action->drivetrain->rate.desiredDriveRate);
		}
		action->drivetrain->setDriveOperation = true;
	}
	else
	{
		action->drivetrain->setDriveOperation = false;
		AsynchronousPrinter::Printf(
				"Previous drive operation not complete, discarding");
	}
	action->drivetrain->previousDriveOperationComplete
			= m_drive_control.driveOperationComplete();

	if (m_drive_control.turnOperationComplete())
	{
		if (action->drivetrain->rate.turn_control)
		{
			m_drive_control.setTurnControl(ClosedLoopDrivetrain::CL_RATE);
			m_drive_control.setTurnRate(
					action->drivetrain->rate.desiredTurnRate);
		}
		else if (action->drivetrain->position.turn_control)
		{
			m_drive_control.setTurnControl(ClosedLoopDrivetrain::CL_POSITION);
			m_drive_control.setRelativeTurnPosition(
					action->drivetrain->position.desiredRelativeTurnPosition);
			// command has been set, so now zero the relative pos
			// this serves as a crude one-command queue
			action->drivetrain->position.desiredRelativeTurnPosition = 0;
		}
		else
		{
			m_drive_control.setTurnControl(ClosedLoopDrivetrain::CL_DISABLED);
			m_drive_control.setRawTurnDutyCycle(
					action->drivetrain->rate.desiredTurnRate);
		}
		action->drivetrain->setTurnOperation = true;
	}
	else
	{
		action->drivetrain->setTurnOperation = false;
		AsynchronousPrinter::Printf(
				"Previous turn operation not complete, discarding");
	}
	action->drivetrain->previousTurnOperationComplete
			= m_drive_control.turnOperationComplete();

	m_drive_control.update();

	DriveCommand cmd = m_drive_control.getOutput();

	// decrease one cycle until zero
	if (action->drivetrain->synchronizedCyclesRemaining > 0)
	{
		action->drivetrain->synchronizedCyclesRemaining--;
		cmd.rightCommand.dutyCycle
				= m_encoders.GetNormalizedOpposingGearMotorSpeed(
						m_encoders.GetRightEncoder());
		cmd.leftCommand.dutyCycle
				= m_encoders.GetNormalizedOpposingGearMotorSpeed(
						m_encoders.GetLeftEncoder());
		cmd.rightCommand.brakingDutyCycle = 0.0;
		cmd.leftCommand.brakingDutyCycle = 0.0;
	}

	m_esc_left->SetDutyCycle(cmd.leftCommand.dutyCycle);
	m_esc_right->SetDutyCycle(cmd.rightCommand.dutyCycle);
	m_esc_left->SetBrake((int) (cmd.leftCommand.brakingDutyCycle * 8));
	m_esc_right->SetBrake((int) (cmd.rightCommand.brakingDutyCycle * 8));

	if (action->wasDisabled)
	{
		m_drive_control.reset();
	}
}
