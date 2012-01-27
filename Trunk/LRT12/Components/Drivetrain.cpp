#include "Drivetrain.h"
#include "../Util/AsyncPrinter.h"
#include "../ActionData/DriveTrainAction.h"
#include "../ActionData/ShifterAction.h"

Drivetrain::Drivetrain() :
	Component(), m_name("Drivetrain"), m_encoders(DriveEncoders::GetInstance())
{
	m_esc_left
			= new Esc(RobotConfig::CAN::DRIVE_LEFT_A,
					RobotConfig::CAN::DRIVE_LEFT_B,
					m_encoders.getLeftEncoder(), "left");
	m_esc_right = new Esc(RobotConfig::CAN::DRIVE_RIGHT_A,
			RobotConfig::CAN::DRIVE_RIGHT_B, m_encoders.getRightEncoder(),
			"right");
}

Drivetrain::~Drivetrain()
{
	delete m_esc_left;
	delete m_esc_right;
}

void Drivetrain::Configure()
{
	Config::GetInstance().Get(m_name, "numCyclesToSync", 25);
}

std::string Drivetrain::GetName()
{
	return m_name;
}

void Drivetrain::Output()
{
	bool isHighGear = action->shifter->gear == ACTION::GEARBOX::HIGH_GEAR;
	if (m_drive_control.getHighGear() != isHighGear)
	{
		action->drivetrain->synchronizedCyclesRemaining = NUM_CYCLES_TO_SYNC;
	}

	//	m_encoders.setHighGear(isHighGear); // set by shifter
	m_drive_control.setHighGear(isHighGear);

	// only try new operation if previous one is complete, else discard
	if (m_drive_control.driveOperationComplete()
			|| action->drivetrain->overrideOperationChecks)
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
		AsyncPrinter::Printf(
				"Previous drive operation not complete, discarding");
	}
	action->drivetrain->previousDriveOperationComplete
			= m_drive_control.driveOperationComplete();

	if (m_drive_control.turnOperationComplete()
			|| action->drivetrain->overrideOperationChecks)
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
		AsyncPrinter::Printf("Previous turn operation not complete, discarding");
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
				= m_encoders.getNormalizedOpposingGearMotorSpeed(
						m_encoders.getRightEncoder());
		cmd.leftCommand.dutyCycle
				= m_encoders.getNormalizedOpposingGearMotorSpeed(
						m_encoders.getLeftEncoder());
		cmd.rightCommand.brakingDutyCycle = 0.0;
		cmd.leftCommand.brakingDutyCycle = 0.0;
	}

	m_esc_left->SetDutyCycle(cmd.leftCommand.dutyCycle);
	m_esc_right->SetDutyCycle(cmd.rightCommand.dutyCycle);
	m_esc_left->SetBrake((int) (cmd.leftCommand.brakingDutyCycle * 8));
	m_esc_right->SetBrake((int) (cmd.rightCommand.brakingDutyCycle * 8));

	action->drivetrain->raw.leftDutyCycle = cmd.leftCommand.dutyCycle;
	action->drivetrain->raw.leftBrakingDutyCycle
			= cmd.leftCommand.brakingDutyCycle;
	action->drivetrain->raw.rightDutyCycle = cmd.rightCommand.dutyCycle;
	action->drivetrain->raw.rightBrakingDutyCycle
			= cmd.rightCommand.brakingDutyCycle;

	if (action->wasDisabled)
	{
		m_drive_control.reset();
	}
}

void Drivetrain::log()
{
	SmartDashboard * sdb = SmartDashboard::GetInstance();
	std::string prefix = m_name + ": ";

	std::string drivemode;
	switch (m_drive_control.getDriveMode())
	{
	case ClosedLoopDrivetrain::CL_DISABLED:
		drivemode = "Open Loop";
		break;
	case ClosedLoopDrivetrain::CL_RATE:
		drivemode = "Rate";
		break;
	case ClosedLoopDrivetrain::CL_POSITION:
		drivemode = "Position";
		break;
	}
	sdb->PutString(prefix + "Drive mode", drivemode);

	std::string turnmode;
	switch (m_drive_control.getTurnMode())
	{
	case ClosedLoopDrivetrain::CL_DISABLED:
		turnmode = "Open Loop";
		break;
	case ClosedLoopDrivetrain::CL_RATE:
		turnmode = "Rate";
		break;
	case ClosedLoopDrivetrain::CL_POSITION:
		turnmode = "Position";
		break;
	}
	sdb->PutString(prefix + "Turn mode", turnmode);

	sdb->PutDouble((prefix + "Left Duty Cycle").c_str(),
			action->drivetrain->raw.leftDutyCycle);
	sdb->PutDouble((prefix + "Right Duty Cycle").c_str(),
			action->drivetrain->raw.rightDutyCycle);
	sdb->PutDouble((prefix + "Left Braking Duty Cycle").c_str(),
			action->drivetrain->raw.leftBrakingDutyCycle);
	sdb->PutDouble((prefix + "Right Braking Duty Cycle").c_str(),
			action->drivetrain->raw.rightBrakingDutyCycle);
}
