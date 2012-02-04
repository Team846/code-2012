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

	printf("Constructed CLRateTrain\n");
}

void ClosedLoopDrivetrain::Configure()
{
	const static string configSection = "CLDriveTrain";

	double drive_rate_p_high = m_config->Get<double> (configSection,
			"driveRateHighP", 1.5);
	double drive_rate_i_high = m_config->Get<double> (configSection,
			"driveRateHighI", 0.0);
	double drive_rate_d_high = m_config->Get<double> (configSection,
			"driveRateHighD", 0.0);

	m_rate_drive_high_gear_pid.setParameters(drive_rate_p_high,
			drive_rate_i_high, drive_rate_d_high, 1.0, FWD_DECAY, true);

	double drive_rate_p_low = m_config->Get<double> (configSection,
			"driveRateLowP", 1.5);
	double drive_rate_i_low = m_config->Get<double> (configSection,
			"driveRateLowI", 0.0);
	double drive_rate_d_low = m_config->Get<double> (configSection,
			"driveRateLowD", 0.0);

	m_rate_drive_low_gear_pid.setParameters(drive_rate_p_low, drive_rate_i_low,
			drive_rate_d_low, 1.0, FWD_DECAY, true);

	double turn_rate_p_high = m_config->Get<double> (configSection,
			"turnRateHighP", 1.5);
	double turn_rate_i_high = m_config->Get<double> (configSection,
			"turnRateHighI", 0.0);
	double turn_rate_d_high = m_config->Get<double> (configSection,
			"turnRateHighD", 0.0);

	m_rate_turn_high_gear_pid.setParameters(turn_rate_p_high, turn_rate_i_high,
			turn_rate_d_high, 1.0, FWD_DECAY, true);

	double turn_rate_p_low = m_config->Get<double> (configSection,
			"turnRateLowP", 1.5);
	double turn_rate_i_low = m_config->Get<double> (configSection,
			"turnRateLowI", 0.0);
	double turn_rate_d_low = m_config->Get<double> (configSection,
			"turnRateLowD", 0.0);

	m_rate_turn_low_gear_pid.setParameters(turn_rate_p_low, turn_rate_i_low,
			turn_rate_d_low, 1.0, FWD_DECAY, true);

	double drive_pos_p_high = m_config->Get<double> (configSection,
			"drivePosHighP", 1.5);
	double drive_pos_i_high = m_config->Get<double> (configSection,
			"drivePosHighI", 0.0);
	double drive_pos_d_high = m_config->Get<double> (configSection,
			"drivePosHighD", 0.0);

	m_pos_drive_high_gear_pid.setParameters(drive_pos_p_high, drive_pos_i_high,
			drive_pos_d_high, 1.0, FWD_DECAY, true);

	double drive_pos_p_low = m_config->Get<double> (configSection,
			"drivePosLowP", 1.5);
	double drive_pos_i_low = m_config->Get<double> (configSection,
			"drivePosLowI", 0.0);
	double drive_pos_d_low = m_config->Get<double> (configSection,
			"drivePosLowD", 0.0);

	m_pos_drive_low_gear_pid.setParameters(drive_pos_p_low, drive_pos_i_low,
			drive_pos_d_low, 1.0, FWD_DECAY, true);

	double turn_pos_p_high = m_config->Get<double> (configSection,
			"turnPosHighP", 1.5);
	double turn_pos_i_high = m_config->Get<double> (configSection,
			"turnPosHighI", 0.0);
	double turn_pos_d_high = m_config->Get<double> (configSection,
			"turnPosHighD", 0.0);

	m_pos_turn_high_gear_pid.setParameters(turn_pos_p_high, turn_pos_i_high,
			turn_pos_d_high, 1.0, FWD_DECAY, true);

	double turn_pos_p_low = m_config->Get<double> (configSection, "turnPosLowP",
			1.5);
	double turn_pos_i_low = m_config->Get<double> (configSection, "turnPosLowI",
			0.0);
	double turn_pos_d_low = m_config->Get<double> (configSection, "turnPosLowD",
			0.0);

	m_pos_turn_low_gear_pid.setParameters(turn_pos_p_low, turn_pos_i_low,
			turn_pos_d_low, 1.0, FWD_DECAY, true);
}

ClosedLoopDrivetrain::DriveCommand ClosedLoopDrivetrain::Drive(double rawTurn,
		double rawFwd)
{
	DriveCommand drive;

	double leftInput = rawTurn - rawFwd;
	double rightInput = rawTurn + rawFwd;

	// TODO must decide whether forward or turn takes precedence
	if (Util::Abs<double>(leftInput) > 1.0)
		leftInput = Util::Sign<double>(leftInput) * 1.0;

	// TODO must decide whether forward or turn takes precedence
	if (Util::Abs<double>(rightInput) > 1.0)
		rightInput = Util::Sign<double>(rightInput) * 1.0;

	drive.leftDutyCycle = leftInput;
	drive.rightDutyCycle = rightInput;
	drive.shouldLinearize = true;
	return drive;
}

ClosedLoopDrivetrain::DriveCommand ClosedLoopDrivetrain::getOutput()
{
	return Drive(m_drive_control->getOutput(), m_turn_control->getOutput());
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
				Util::Clamp<double>(m_encoders.getNormalizedForwardMotorSpeed(),
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
				Util::Clamp<double>(m_encoders.getNormalizedTurningMotorSpeed(),
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
		break;
	case CL_RATE:
		m_drive_control = m_in_high_gear ? &m_rate_drive_high_gear_pid
				: &m_rate_drive_low_gear_pid;
		break;
	case CL_POSITION:
		m_drive_control = m_in_high_gear ? &m_pos_drive_high_gear_pid
				: &m_pos_drive_low_gear_pid;
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
		AsyncPrinter::Printf("Disabled closed-loop turn\n");
		break;
	case CL_RATE:
		m_turn_control = m_in_high_gear ? &m_rate_turn_high_gear_pid
				: &m_rate_turn_low_gear_pid;
		AsyncPrinter::Printf("Enabled closed-loop rate control on turn\n");
		break;
	case CL_POSITION:
		m_turn_control = m_in_high_gear ? &m_pos_turn_high_gear_pid
				: &m_pos_turn_low_gear_pid;
		AsyncPrinter::Printf("Enabled closed-loop position control on turn\n");
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

void ClosedLoopDrivetrain::setRelativeDrivePosition(double pos)
{
	setDriveControl(CL_POSITION);
	m_drive_control->setSetpoint(pos + m_encoders.getRobotDist());
	m_drive_op_complete = false;
}

void ClosedLoopDrivetrain::setDriveRate(double rate)
{
	setDriveControl(CL_RATE);
	m_drive_control->setSetpoint(rate);
	m_drive_op_complete = false;
}

double ClosedLoopDrivetrain::getDriveSetpoint()
{
	return m_drive_control->getSetpoint();
}

void ClosedLoopDrivetrain::setRawDriveDutyCycle(double duty)
{
	setDriveControl(CL_DISABLED);
	m_drive_control->setSetpoint(duty);
	m_drive_op_complete = false;
}

void ClosedLoopDrivetrain::setRelativeTurnPosition(double pos)
{
	setTurnControl(CL_POSITION);
	m_turn_control->setSetpoint(pos + m_encoders.getTurnAngle());
	m_turn_op_complete = false;
}

void ClosedLoopDrivetrain::setTurnRate(double rate)
{
	setTurnControl(CL_RATE);
	m_turn_control->setSetpoint(rate);
	m_turn_op_complete = false;
}

double ClosedLoopDrivetrain::getTurnSetpoint()
{
	return m_turn_control->getSetpoint();
}

void ClosedLoopDrivetrain::setRawTurnDutyCycle(double duty)
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
