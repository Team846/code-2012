#include "Drivetrain.h"
#include "../Util/AsyncPrinter.h"
#include "../ActionData/DriveTrainAction.h"
#include "../ActionData/ShifterAction.h"
#include "../ActionData/IMUData.h"

Drivetrain::Drivetrain() :
	Component(), Configurable(), m_name("Drivetrain"),
			m_encoders(DriveEncoders::GetInstance())
{
	m_esc_left
			= new ESC(RobotConfig::CAN::DRIVE_LEFT_A,
					RobotConfig::CAN::DRIVE_LEFT_B,
					m_encoders.getLeftEncoder(), "left");
	m_esc_right = new ESC(RobotConfig::CAN::DRIVE_RIGHT_A,
			RobotConfig::CAN::DRIVE_RIGHT_B, m_encoders.getRightEncoder(),
			"right");
	m_delta_power_limit = m_forward_accel_limit = m_reverse_accel_limit
			= NUM_CYCLES_TO_SYNC = 0;
	Configure();
}

Drivetrain::~Drivetrain()
{
	delete m_esc_left;
	delete m_esc_right;
}

void Drivetrain::Configure()
{
	Config * c = Config::GetInstance();
	NUM_CYCLES_TO_SYNC = c->Get(m_name, "numCyclesToSync", 25);
	m_forward_accel_limit = c->Get(m_name, "forwardAccelLimit", 4.9);
	m_reverse_accel_limit = c->Get(m_name, "reverseAccelLimit", -1.0);
	m_delta_power_limit = c->Get(m_name, "deltaPowerLimit", 0.25);
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
	static bool wasHighGear = false;
	bool isHighGear = m_action->shifter->gear == ACTION::GEARBOX::HIGH_GEAR;
	if (wasHighGear != isHighGear)
	{
		m_action->drivetrain->synchronizedCyclesRemaining = NUM_CYCLES_TO_SYNC;
	}
	wasHighGear = isHighGear;

	m_drive_control.setHighGear(isHighGear);

	// set position zeros if necessary
	if (m_action->drivetrain->position.reset_translate_zero)
	{
		m_action->drivetrain->position.reset_translate_zero = false;
		m_drive_control.SetCurrentTranslatePositionAsZero();
	}

	if (m_action->drivetrain->position.reset_turn_zero)
	{
		m_action->drivetrain->position.reset_turn_zero = false;
		m_drive_control.SetCurrentTurnPositionAsZero();
	}

	// check actual drive commands
	if (m_action->drivetrain->position.drive_control)
	{
		if (m_action->drivetrain->position.absoluteTranslate)
		{
			m_drive_control.setAbsoluteTranslatePosition(
					m_action->drivetrain->position.desiredAbsoluteDrivePosition);
		}
		else if (m_drive_control.driveOperationComplete()
				&& m_action->drivetrain->position.desiredRelativeDrivePosition
						!= ACTION::DRIVETRAIN::UNSET_POS)
		{
			AsyncPrinter::Printf("Op done\n");
			m_drive_control.setRelativeTranslatePosition(
					m_action->drivetrain->position.desiredRelativeDrivePosition);

			// command has been set, so now zero the relative pos
			// this serves as a crude one-command queue
			m_action->drivetrain->position.desiredRelativeDrivePosition
					= ACTION::DRIVETRAIN::UNSET_POS;
		}
		m_drive_control.setTranslateControl(ClosedLoopDrivetrain::CL_POSITION);
	}
	else if (m_action->drivetrain->rate.drive_control)
	{
		m_drive_control.setTranslateControl(ClosedLoopDrivetrain::CL_RATE);
		m_drive_control.setTranslateRate(
				m_action->drivetrain->rate.desiredDriveRate);
	}
	else
	{
		m_drive_control.setTranslateControl(ClosedLoopDrivetrain::CL_DISABLED);
		m_drive_control.setTranslateDriveDutyCycle(
				m_action->drivetrain->rate.desiredDriveRate);
	}
	m_action->drivetrain->setDriveOperation = true;
	m_action->drivetrain->previousDriveOperationComplete
			= m_drive_control.driveOperationComplete();

	if (m_action->drivetrain->rate.turn_control)
	{
		m_drive_control.setTurnControl(ClosedLoopDrivetrain::CL_RATE);
		m_drive_control.setTurnRate(m_action->drivetrain->rate.desiredTurnRate);
	}
	else if (m_action->drivetrain->position.turn_control)
	{
		if (m_action->drivetrain->position.absoluteTurn)
		{
			m_drive_control.setAbsoluteTurnPosition(
					m_action->drivetrain->position.desiredAbsoluteTurnPosition);
		}
		else if (m_drive_control.turnOperationComplete()
				&& m_action->drivetrain->position.desiredRelativeTurnPosition
						!= ACTION::DRIVETRAIN::UNSET_POS)
		{
			m_drive_control.setRelativeTurnPosition(
					m_action->drivetrain->position.desiredRelativeTurnPosition);
			// command has been set, so now zero the relative pos
			// this serves as a crude one-command queue
			m_action->drivetrain->position.desiredRelativeTurnPosition
					= ACTION::DRIVETRAIN::UNSET_POS;
		}
		m_drive_control.setTurnControl(ClosedLoopDrivetrain::CL_POSITION);
	}
	else
	{
		m_drive_control.setTurnControl(ClosedLoopDrivetrain::CL_DISABLED);
		m_drive_control.setRawTurnDutyCycle(
				m_action->drivetrain->rate.desiredTurnRate);
	}
	m_action->drivetrain->setTurnOperation = true;
	m_action->drivetrain->previousTurnOperationComplete
			= m_drive_control.turnOperationComplete();

	m_drive_control.update();

	ClosedLoopDrivetrain::DriveCommand cmd = m_drive_control.getOutput();

	if (m_action->imu->accel_x > m_forward_accel_limit
			|| m_action->imu->accel_x < m_reverse_accel_limit)
	{
		double lspeed = m_encoders.getNormalizedMotorSpeed(
				m_encoders.getLeftEncoder());
		double rspeed = m_encoders.getNormalizedMotorSpeed(
				m_encoders.getRightEncoder());
		if (fabs(cmd.leftDutyCycle - lspeed) > m_delta_power_limit)
		{
			cmd.leftDutyCycle = lspeed + Util::Sign<double>(
					cmd.leftDutyCycle - lspeed) * m_delta_power_limit;
		}

		if (fabs(cmd.rightDutyCycle - rspeed) > m_delta_power_limit)
		{
			cmd.rightDutyCycle = rspeed + Util::Sign<double>(
					cmd.rightDutyCycle - rspeed) * m_delta_power_limit;
		}
	}

	// decrease one cycle until zero
	if (m_action->drivetrain->synchronizedCyclesRemaining > 0)
	{
		m_action->drivetrain->synchronizedCyclesRemaining--;
		cmd.rightDutyCycle = m_encoders.getNormalizedOpposingGearMotorSpeed(
				m_encoders.getRightEncoder());
		cmd.leftDutyCycle = m_encoders.getNormalizedOpposingGearMotorSpeed(
				m_encoders.getLeftEncoder());

		cmd.shouldLinearize = false;
	}

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
	//	m_action_ptr->drivetrain->raw.leftBrakingDutwyCycle
	//			= cmd.leftCommand.brakingDutyCycle;
	m_action->drivetrain->raw.rightDutyCycle = cmd.rightDutyCycle;
	//	m_action_ptr->drivetrain->raw.rightBrakingDutyCycle
	//			= cmd.rightCommand.brakingDutyCycle;

	if (m_action->wasDisabled)
	{
		m_drive_control.reset();
	}

	m_action->drivetrain->robotRotateSpeed
			= m_encoders.getNormalizedTurningMotorSpeed();
	m_action->drivetrain->robotTranslateSpeed
			= m_encoders.getNormalizedForwardMotorSpeed();
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
