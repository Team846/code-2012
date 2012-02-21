#include "Drivetrain.h"
#include "../Util/AsyncPrinter.h"
#include "../ActionData/DriveTrainAction.h"
#include "../ActionData/ShifterAction.h"

Drivetrain::Drivetrain() :
	Component(), m_name("Drivetrain"), m_encoders(DriveEncoders::GetInstance())
{
	m_esc_left
			= new ESC(RobotConfig::CAN::DRIVE_LEFT_A,
					RobotConfig::CAN::DRIVE_LEFT_B,
					m_encoders.getLeftEncoder(), "left");
	m_esc_right = new ESC(RobotConfig::CAN::DRIVE_RIGHT_A,
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
	Config::GetInstance()->Get(m_name, "numCyclesToSync", 25);
}

std::string Drivetrain::GetName()
{
	return m_name;
}

void Drivetrain::Disable()
{
	m_esc_left->Disable();
	m_esc_right->Disable();
}

void Drivetrain::Output()
{
	bool isHighGear = m_action->shifter->gear == ACTION::GEARBOX::HIGH_GEAR;
	if (m_drive_control.getHighGear() != isHighGear)
	{
		m_action->drivetrain->synchronizedCyclesRemaining = NUM_CYCLES_TO_SYNC;
	}

	//	m_encoders.setHighGear(isHighGear); // set by shifter
	m_drive_control.setHighGear(isHighGear);

	// only try new operation if previous one is complete, else discard
	if (m_drive_control.driveOperationComplete()
			|| m_action->drivetrain->overrideOperationChecks)
	{
		if (m_action->drivetrain->position.drive_control)
		{
			m_drive_control.setTranslateControl(
					ClosedLoopDrivetrain::CL_POSITION);
			m_drive_control.setRelativeTranslatePosition(
					m_action->drivetrain->position.desiredRelativeDrivePosition);
			if (m_action->drivetrain->position.desiredRelativeDrivePosition
					> 0.01)
				AsyncPrinter::Printf(
						"setpoint %.2f\n",
						m_action->drivetrain->position.desiredRelativeDrivePosition);
			// command has been set, so now zero the relative pos
			// this serves as a crude one-command queue
			m_action->drivetrain->position.desiredRelativeDrivePosition = 0;
		}
		else if (m_action->drivetrain->rate.drive_control)
		{
			m_drive_control.setTranslateControl(ClosedLoopDrivetrain::CL_RATE);
			m_drive_control.setTranslateRate(
					m_action->drivetrain->rate.desiredDriveRate);
		}
		else
		{
			m_drive_control.setTranslateControl(
					ClosedLoopDrivetrain::CL_DISABLED);
			m_drive_control.setTranslateDriveDutyCycle(
					m_action->drivetrain->rate.desiredDriveRate);
		}
		m_action->drivetrain->setDriveOperation = true;
	}
	else
	{
		m_action->drivetrain->setDriveOperation = false;
		AsyncPrinter::Printf(
				"Previous drive operation not complete, discarding\n");
	}
	m_action->drivetrain->previousDriveOperationComplete
			= m_drive_control.driveOperationComplete();

	if (m_drive_control.turnOperationComplete()
			|| m_action->drivetrain->overrideOperationChecks)
	{
		if (m_action->drivetrain->rate.turn_control)
		{
			m_drive_control.setTurnControl(ClosedLoopDrivetrain::CL_RATE);
			m_drive_control.setTurnRate(
					m_action->drivetrain->rate.desiredTurnRate);
		}
		else if (m_action->drivetrain->position.turn_control)
		{
			m_drive_control.setTurnControl(ClosedLoopDrivetrain::CL_POSITION);
			m_drive_control.setRelativeTurnPosition(
					m_action->drivetrain->position.desiredRelativeTurnPosition);
			// command has been set, so now zero the relative pos
			// this serves as a crude one-command queue
			m_action->drivetrain->position.desiredRelativeTurnPosition = 0;
		}
		else
		{
			m_drive_control.setTurnControl(ClosedLoopDrivetrain::CL_DISABLED);
			m_drive_control.setRawTurnDutyCycle(
					m_action->drivetrain->rate.desiredTurnRate);
		}
		m_action->drivetrain->setTurnOperation = true;
	}
	else
	{
		m_action->drivetrain->setTurnOperation = false;
		AsyncPrinter::Printf(
				"Previous turn operation not complete, discarding\n");
	}
	m_action->drivetrain->previousTurnOperationComplete
			= m_drive_control.turnOperationComplete();

	m_drive_control.update();

	ClosedLoopDrivetrain::DriveCommand cmd = m_drive_control.getOutput();

	// decrease one cycle until zero
	// DIS DOESN"T WORK -RY, BA
	//	if (m_action_ptr->drivetrain->synchronizedCyclesRemaining > 0)
	//	{
	//		m_action_ptr->drivetrain->synchronizedCyclesRemaining--;
	//		cmd.rightDutyCycle = m_encoders.getNormalizedOpposingGearMotorSpeed(
	//				m_encoders.getRightEncoder());
	//		cmd.leftDutyCycle = m_encoders.getNormalizedOpposingGearMotorSpeed(
	//				m_encoders.getLeftEncoder());
	//
	//		cmd.shouldLinearize = false;
	//	}

	if (m_action->motorsEnabled)
	{
		m_esc_left->SetDutyCycle(cmd.leftDutyCycle);
		m_esc_right->SetDutyCycle(cmd.rightDutyCycle);
	}
	else
	{
		m_esc_left->SetDutyCycle(0.0);
		m_esc_right->SetDutyCycle(0.0);
	}

	m_action->drivetrain->raw.leftDutyCycle = cmd.leftDutyCycle;
	//	m_action_ptr->drivetrain->raw.leftBrakingDutyCycle
	//			= cmd.leftCommand.brakingDutyCycle;
	m_action->drivetrain->raw.rightDutyCycle = cmd.rightDutyCycle;
	//	m_action_ptr->drivetrain->raw.rightBrakingDutyCycle
	//			= cmd.rightCommand.brakingDutyCycle;

	if (m_action->wasDisabled)
	{
		m_drive_control.reset();
	}
}

void Drivetrain::log()
{
	SmartDashboard * sdb = SmartDashboard::GetInstance();
	std::string prefix = m_name + ": ";

	sdb->PutDouble((prefix + "Left Duty Cycle").c_str(),
			m_action->drivetrain->raw.leftDutyCycle);
	sdb->PutDouble((prefix + "Right Duty Cycle").c_str(),
			m_action->drivetrain->raw.rightDutyCycle);
}
