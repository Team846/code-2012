#include "CLDriveTrain.h"
#include "../../Util/AsyncPrinter.h"
#include <math.h>

ClosedLoopDrivetrain::ClosedLoopDrivetrain() :
	m_encoders(DriveEncoders::GetInstance()), m_config(Config::GetInstance()),
			m_brake_left(false), m_brake_right(false),
			m_drive_control_type(CL_RATE), m_turn_control_type(CL_RATE),
			m_in_high_gear(true)
{
	Configure();
	// disable these to make code simpler
	m_drive_disabled.disablePID();
	m_turn_disabled.disablePID();

	disableLog();

	printf("Constructed CLRateTrain\n");
}

void ClosedLoopDrivetrain::Configure()
{
	const static string configSection = "CLDriveTrain";

	float drive_rate_p_high = m_config->Get<float> (configSection,
			"driveRateHighP", 1.5);
	float drive_rate_i_high = m_config->Get<float> (configSection,
			"driveRateHighI", 0.0);
	float drive_rate_d_high = m_config->Get<float> (configSection,
			"driveRateHighD", 0.0);

	m_rate_drive_high_gear_pid.setParameters(drive_rate_p_high,
			drive_rate_i_high, drive_rate_d_high, 1.0, FWD_DECAY, true);

	float drive_rate_p_low = m_config->Get<float> (configSection,
			"driveRateLowP", 1.5);
	float drive_rate_i_low = m_config->Get<float> (configSection,
			"driveRateLowI", 0.0);
	float drive_rate_d_low = m_config->Get<float> (configSection,
			"driveRateLowD", 0.0);

	m_rate_drive_low_gear_pid.setParameters(drive_rate_p_low, drive_rate_i_low,
			drive_rate_d_low, 1.0, FWD_DECAY, true);

	float turn_rate_p_high = m_config->Get<float> (configSection,
			"turnRateHighP", 1.5);
	float turn_rate_i_high = m_config->Get<float> (configSection,
			"turnRateHighI", 0.0);
	float turn_rate_d_high = m_config->Get<float> (configSection,
			"turnRateHighD", 0.0);

	m_rate_turn_high_gear_pid.setParameters(turn_rate_p_high, turn_rate_i_high,
			turn_rate_d_high, 1.0, FWD_DECAY, true);

	float turn_rate_p_low = m_config->Get<float> (configSection, "turnRateLowP",
			1.5);
	float turn_rate_i_low = m_config->Get<float> (configSection, "turnRateLowI",
			0.0);
	float turn_rate_d_low = m_config->Get<float> (configSection, "turnRateLowD",
			0.0);

	m_rate_turn_low_gear_pid.setParameters(turn_rate_p_low, turn_rate_i_low,
			turn_rate_d_low, 1.0, FWD_DECAY, true);

	float drive_pos_p_high = m_config->Get<float> (configSection,
			"drivePosHighP", 1.5);
	float drive_pos_i_high = m_config->Get<float> (configSection,
			"drivePosHighI", 0.0);
	float drive_pos_d_high = m_config->Get<float> (configSection,
			"drivePosHighD", 0.0);

	m_pos_drive_high_gear_pid.setParameters(drive_pos_p_high, drive_pos_i_high,
			drive_pos_d_high, 1.0, FWD_DECAY, true);

	float drive_pos_p_low = m_config->Get<float> (configSection, "drivePosLowP",
			1.5);
	float drive_pos_i_low = m_config->Get<float> (configSection, "drivePosLowI",
			0.0);
	float drive_pos_d_low = m_config->Get<float> (configSection, "drivePosLowD",
			0.0);

	m_pos_drive_low_gear_pid.setParameters(drive_pos_p_low, drive_pos_i_low,
			drive_pos_d_low, 1.0, FWD_DECAY, true);

	float turn_pos_p_high = m_config->Get<float> (configSection, "turnPosHighP",
			1.5);
	float turn_pos_i_high = m_config->Get<float> (configSection, "turnPosHighI",
			0.0);
	float turn_pos_d_high = m_config->Get<float> (configSection, "turnPosHighD",
			0.0);

	m_pos_turn_high_gear_pid.setParameters(turn_pos_p_high, turn_pos_i_high,
			turn_pos_d_high, 1.0, FWD_DECAY, true);

	float turn_pos_p_low = m_config->Get<float> (configSection, "turnPosLowP",
			1.5);
	float turn_pos_i_low = m_config->Get<float> (configSection, "turnPosLowI",
			0.0);
	float turn_pos_d_low = m_config->Get<float> (configSection, "turnPosLowD",
			0.0);

	m_pos_turn_low_gear_pid.setParameters(turn_pos_p_low, turn_pos_i_low,
			turn_pos_d_low, 1.0, FWD_DECAY, true);
}

ClosedLoopDrivetrain::DriveCommand ClosedLoopDrivetrain::Drive(float rawTurn, float rawFwd)
{
	DriveCommand drive;

	float leftInput = rawTurn - rawFwd;
	float rightInput = rawTurn + rawFwd;

	// TODO must decide whether forward or turn takes precedence
	if(Util::Abs<float>(leftInput) > 1.0)
		leftInput = Util::Sign<float>(leftInput) * 1.0;

	// TODO must decide whether forward or turn takes precedence
	if(Util::Abs<float>(rightInput) > 1.0)
		rightInput = Util::Sign<float>(rightInput) * 1.0;

	drive.leftDutyCycle   = leftInput;
	drive.rightDutyCycle  = rightInput;
	drive.shouldLinearize = true;
	return drive;
}

ClosedLoopDrivetrain::DriveCommand ClosedLoopDrivetrain::getOutput()
{
	return Drive(m_drive_control->getOutput(),
			m_turn_control->getOutput());
}

void ClosedLoopDrivetrain::update()
{
	if (m_drive_control_type == CL_POSITION)
	{
		m_drive_control->setInput(m_encoders.getRobotDist());
	}
	else
	{
		m_drive_control->setInput(
				Util::Clamp<float>(m_encoders.getNormalizedForwardMotorSpeed(),
						-1.0, 1.0));
		m_drive_op_complete = true; // this flag doesn't mean much here
	}
	m_drive_control->update(1.0 / 50); // 50 Hz assumed update rate

	// this only means something for position control.
	if (m_drive_control->getError() < 0.5
			&& m_drive_control->getAccumulatedError() < 5.0e-2)
	{
		m_drive_op_complete = true;
	}

	if (m_turn_control_type == CL_POSITION)
	{
		m_turn_control->setInput(fmod(m_encoders.getTurnAngle(), 360.0));
	}
	else
	{
		m_turn_control->setInput(
				Util::Clamp<float>(m_encoders.getNormalizedTurningMotorSpeed(),
						-1.0, 1.0));
		m_turn_op_complete = true; // this flag doesn't mean much here
	}
	m_turn_control->update(1.0 / 50); // 50 Hz assumed update rate

	// this only means something for position control.
	if (m_turn_control->getError() < 0.5
			&& m_turn_control->getAccumulatedError() < 5.0e-2)
	{
		m_turn_op_complete = true;
	}
}

void ClosedLoopDrivetrain::setDriveControl(CONTROL_TYPE type)
{
	if (m_drive_control_type != type)
	{
		m_rate_drive_high_gear_pid.reset();
		m_rate_drive_low_gear_pid.reset();
		m_pos_drive_high_gear_pid.reset();
		m_pos_drive_low_gear_pid.reset();
	}

	m_drive_control_type = type;

	switch (m_drive_control_type)
	{
	default:
	case CL_DISABLED:
		m_drive_control = &m_drive_disabled;
		AsyncPrinter::Printf("Disabled closed-loop drive");
		break;
	case CL_RATE:
		m_drive_control = m_in_high_gear ? &m_rate_drive_high_gear_pid
				: &m_rate_drive_low_gear_pid;
		AsyncPrinter::Printf("Enabled closed-loop rate control on drive");
		break;
	case CL_POSITION:
		m_drive_control = m_in_high_gear ? &m_pos_drive_high_gear_pid
				: &m_pos_drive_low_gear_pid;
		AsyncPrinter::Printf("Enabled closed-loop position control on drive");
		break;
	}
}

void ClosedLoopDrivetrain::setTurnControl(CONTROL_TYPE type)
{
	if (m_turn_control_type != type)
	{
		m_rate_turn_high_gear_pid.reset();
		m_rate_turn_low_gear_pid.reset();
		m_pos_turn_high_gear_pid.reset();
		m_pos_turn_low_gear_pid.reset();
	}

	m_turn_control_type = type;

	switch (m_turn_control_type)
	{
	default:
	case CL_DISABLED:
		m_turn_control = &m_turn_disabled;
		AsyncPrinter::Printf("Disabled closed-loop turn");
		break;
	case CL_RATE:
		m_turn_control = m_in_high_gear ? &m_rate_turn_high_gear_pid
				: &m_rate_turn_low_gear_pid;
		AsyncPrinter::Printf("Enabled closed-loop rate control on turn");
		break;
	case CL_POSITION:
		m_turn_control = m_in_high_gear ? &m_pos_turn_high_gear_pid
				: &m_pos_turn_low_gear_pid;
		AsyncPrinter::Printf("Enabled closed-loop position control on turn");
		break;
	}
}

void ClosedLoopDrivetrain::setHighGear(bool isHighGear)
{
	if (m_in_high_gear != isHighGear)
	{
		m_in_high_gear = isHighGear;
		setDriveControl(m_drive_control_type);
		setTurnControl(m_turn_control_type);
		reset();
	}
}

bool ClosedLoopDrivetrain::getHighGear()
{
	return m_in_high_gear;
}

void ClosedLoopDrivetrain::setRelativeDrivePosition(float pos)
{
	setDriveControl(CL_POSITION);
	m_drive_control->setSetpoint(pos + m_encoders.getRobotDist());
	m_drive_op_complete = false;
}

void ClosedLoopDrivetrain::setDriveRate(float rate)
{
	setDriveControl(CL_RATE);
	m_drive_control->setSetpoint(rate);
	m_drive_op_complete = false;
}

float ClosedLoopDrivetrain::getDriveSetpoint()
{
	return m_drive_control->getSetpoint();
}

void ClosedLoopDrivetrain::setRawDriveDutyCycle(float duty)
{
	setDriveControl(CL_DISABLED);
	m_drive_control->setSetpoint(duty);
	m_drive_op_complete = false;
}

void ClosedLoopDrivetrain::setRelativeTurnPosition(float pos)
{
	setTurnControl(CL_POSITION);
	m_turn_control->setSetpoint(pos + m_encoders.getTurnAngle());
	m_turn_op_complete = false;
}

void ClosedLoopDrivetrain::setTurnRate(float rate)
{
	setTurnControl(CL_RATE);
	m_turn_control->setSetpoint(rate);
	m_turn_op_complete = false;
}

float ClosedLoopDrivetrain::getTurnSetpoint()
{
	return m_turn_control->getSetpoint();
}

void ClosedLoopDrivetrain::setRawTurnDutyCycle(float duty)
{
	setTurnControl(CL_DISABLED);
	m_turn_control->setSetpoint(duty);
	m_turn_op_complete = false;
}

bool ClosedLoopDrivetrain::driveOperationComplete()
{
	return m_drive_op_complete;
}

bool ClosedLoopDrivetrain::turnOperationComplete()
{
	return m_turn_op_complete;
}

ClosedLoopDrivetrain::CONTROL_TYPE ClosedLoopDrivetrain::getDriveMode()
{
	return m_drive_control_type;
}

ClosedLoopDrivetrain::CONTROL_TYPE ClosedLoopDrivetrain::getTurnMode()
{
	return m_turn_control_type;
}

void ClosedLoopDrivetrain::reset()
{
	m_rate_drive_high_gear_pid.reset();
	m_rate_drive_low_gear_pid.reset();
	m_pos_drive_high_gear_pid.reset();
	m_pos_drive_low_gear_pid.reset();
	m_rate_turn_high_gear_pid.reset();
	m_rate_turn_low_gear_pid.reset();
	m_pos_turn_high_gear_pid.reset();
	m_pos_turn_low_gear_pid.reset();
}

void ClosedLoopDrivetrain::log()
{
	SmartDashboard * sdb = SmartDashboard::GetInstance();

	std::string drivemode;
	switch (getDriveMode())
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
	sdb->PutString("Drive mode", drivemode);
	sdb->PutDouble("Active Drive P Gain",
			m_drive_control->getProportionalGain());
	sdb->PutDouble("Active Drive I Gain", m_drive_control->getIntegralGain());
	sdb->PutDouble("Active Drive D Gain", m_drive_control->getDerivativeGain());
	sdb->PutDouble("Active Drive Error", m_drive_control->getError());
	sdb->PutDouble("Active Drive Accumulated Error",
			m_drive_control->getAccumulatedError());

	std::string turnmode;
	switch (getTurnMode())
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
	sdb->PutString("Turn mode", turnmode);
	sdb->PutDouble("Active Turn P Gain", m_turn_control->getProportionalGain());
	sdb->PutDouble("Active Turn I Gain", m_turn_control->getIntegralGain());
	sdb->PutDouble("Active Turn D Gain", m_turn_control->getDerivativeGain());
	sdb->PutDouble("Active Turn Error", m_turn_control->getError());
	sdb->PutDouble("Active Turn Accumulated Error",
			m_turn_control->getAccumulatedError());
}
