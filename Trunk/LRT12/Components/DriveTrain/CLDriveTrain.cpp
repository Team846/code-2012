#include "CLDriveTrain.h"
#include "../../Util/AsyncPrinter.h"
#include <math.h>

ClosedLoopDrivetrain::ClosedLoopDrivetrain() :
	m_encoders(DriveEncoders::GetInstance()), m_config(Config::GetInstance()),
			m_brake_left(false), m_brake_right(false),
			m_fwd_control_type(CL_RATE), m_turn_control_type(CL_RATE),
			m_in_high_gear(true)
{
	Configure();
	// disable these to make code simpler
	m_drive_disabled.disablePID();
	m_turn_disabled.disablePID();

	setDriveControl(m_fwd_control_type);
	setTurnControl(m_turn_control_type);
	
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
			"dri	vePosLowD", 0.0);

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

	double turn_pos_p_low = m_config->Get<double> (configSection,
			"turnPosLowP", 1.5);
	double turn_pos_i_low = m_config->Get<double> (configSection,
			"turnPosLowI", 0.0);
	double turn_pos_d_low = m_config->Get<double> (configSection,
			"turnPosLowD", 0.0);

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
	return Drive(output[FWD], output[TURN]);
}

void ClosedLoopDrivetrain::update()
{
	switch (m_fwd_control_type)
	{
	case CL_POSITION:
		m_pos_control[FWD]->setInput(m_encoders.getRobotDist());
		m_pos_control[FWD]->update(1.0/50.0);
		m_rate_control[FWD]->setSetpoint(m_pos_control[FWD]->getOutput());
		if (m_pos_control[FWD]->getError() < 0.5 
				&& m_pos_control[FWD]->getAccumulatedError() < 5.02-2)
		{
			m_fwd_op_complete = true;
		}
	case CL_RATE:
		m_rate_control[FWD]->setInput(Util::Clamp<double>(
				m_encoders.getNormalizedForwardMotorSpeed(), -1.0, 1.0));
		m_rate_control[FWD]->update(1.0/50);
		output[FWD] = m_rate_control[FWD]->getOutput();
		break;
	case CL_DISABLED:
		m_fwd_op_complete = true;
		break;
	}

	if (m_fwd_control_type != CL_POSITION)
		m_fwd_op_complete = true; // this flag doesn't mean much here

	switch (m_turn_control_type)
	{
	case CL_POSITION:
		m_pos_control[TURN]->setInput(fmod(m_encoders.getTurnAngle(), 360.0));
		m_pos_control[TURN]->update(1.0/50.0);
		m_rate_control[TURN]->setSetpoint(m_pos_control[TURN]->getOutput());
		if (m_pos_control[TURN]->getError() < 0.5 
				&& m_pos_control[TURN]->getAccumulatedError() < 5.02-2)
		{
			m_turn_op_complete = true;
		}
	case CL_RATE:
		m_rate_control[TURN]->setInput(
				Util::Clamp<double>(
						m_encoders.getNormalizedTurningMotorSpeed(), -1.0, 1.0));
		m_rate_control[TURN]->update(1.0/50);
		output[TURN] = m_rate_control[TURN]->getOutput();
		break;
	case CL_DISABLED:
		m_fwd_op_complete = true;
		break;
	}
	if (m_turn_control_type != CL_POSITION)
		m_turn_op_complete = true; // this flag doesn't mean much here
}

void ClosedLoopDrivetrain::setDriveControl(CONTROL_TYPE type)
{
	if (m_fwd_control_type != type)
	{
		m_rate_drive_high_gear_pid.reset();
		m_rate_drive_low_gear_pid.reset();
		m_pos_drive_high_gear_pid.reset();
		m_pos_drive_low_gear_pid.reset();
	}

	m_fwd_control_type = type;

	m_rate_control[FWD] = m_in_high_gear ? &m_rate_drive_high_gear_pid
			: &m_rate_drive_low_gear_pid;
	
	m_pos_control[FWD] = m_in_high_gear ? &m_pos_drive_high_gear_pid
			: &m_pos_drive_low_gear_pid;
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

	m_rate_control[TURN] = m_in_high_gear ? &m_rate_turn_high_gear_pid
			: &m_rate_turn_low_gear_pid;
	m_pos_control[TURN] = m_in_high_gear ? &m_pos_turn_high_gear_pid
			: &m_pos_turn_low_gear_pid;
}

void ClosedLoopDrivetrain::setHighGear(bool isHighGear)
{
	if (m_in_high_gear != isHighGear)
	{
		m_in_high_gear = isHighGear;
		setDriveControl(m_fwd_control_type);
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
	m_pos_control[FWD]->setSetpoint(pos + m_encoders.getRobotDist());
	m_fwd_op_complete = false;
}

void ClosedLoopDrivetrain::setDriveRate(double rate)
{
	setDriveControl(CL_RATE);
	m_rate_control[FWD]->setSetpoint(rate);
	m_fwd_op_complete = false;
}

double ClosedLoopDrivetrain::getDriveSetpoint()
{
	return m_rate_control[FWD]->getSetpoint();
}

void ClosedLoopDrivetrain::setRawDriveDutyCycle(double duty)
{
	setDriveControl(CL_DISABLED);
	output[FWD] = duty;
}

void ClosedLoopDrivetrain::setRelativeTurnPosition(double pos)
{
	setTurnControl(CL_POSITION);
	m_pos_control[TURN]->setSetpoint(pos + m_encoders.getTurnAngle());
	m_turn_op_complete = false;
}

void ClosedLoopDrivetrain::setTurnRate(double rate)
{
	setTurnControl(CL_RATE);
	m_rate_control[TURN]->setSetpoint(rate);
}

double ClosedLoopDrivetrain::getTurnSetpoint()
{
	return m_pos_control[TURN]->getSetpoint();
}

void ClosedLoopDrivetrain::setRawTurnDutyCycle(double duty)
{
	setTurnControl(CL_DISABLED);
	output[TURN] = duty;
}

bool ClosedLoopDrivetrain::driveOperationComplete()
{
	return m_fwd_op_complete;
}

bool ClosedLoopDrivetrain::turnOperationComplete()
{
	return m_turn_op_complete;
}

ClosedLoopDrivetrain::CONTROL_TYPE ClosedLoopDrivetrain::getFwdMode()
{
	return m_fwd_control_type;
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
	switch (getFwdMode())
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

	sdb->PutDouble("FWD Pos Drive P Gain",
			m_pos_control[FWD]->getProportionalGain());
	sdb->PutDouble("FWD Pos Drive I Gain", m_pos_control[FWD]->getIntegralGain());
	sdb->PutDouble("FWD Pos Drive D Gain", m_pos_control[FWD]->getDerivativeGain());
	sdb->PutDouble("FWD Pos Drive Error", m_pos_control[FWD]->getError());
	sdb->PutDouble("FWD Pos Drive Accumulated Error",
			m_pos_control[FWD]->getAccumulatedError());

	sdb->PutDouble("TURN Pos Drive P Gain",
			m_pos_control[TURN]->getProportionalGain());
	sdb->PutDouble("TURN Pos Drive I Gain", m_pos_control[TURN]->getIntegralGain());
	sdb->PutDouble("TURN Pos Drive D Gain", m_pos_control[TURN]->getDerivativeGain());
	sdb->PutDouble("TURN Pos Drive Error", m_pos_control[TURN]->getError());
	sdb->PutDouble("TURN Pos Drive Accumulated Error",
			m_pos_control[TURN]->getAccumulatedError());

	
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
	sdb->PutDouble("FWD rate Drive P Gain",
			m_rate_control[FWD]->getProportionalGain());
	sdb->PutDouble("FWD rate Drive I Gain", m_rate_control[FWD]->getIntegralGain());
	sdb->PutDouble("FWD rate Drive D Gain", m_rate_control[FWD]->getDerivativeGain());
	sdb->PutDouble("FWD rate Drive Error", m_rate_control[FWD]->getError());
	sdb->PutDouble("FWD rate Drive Accumulated Error",
			m_rate_control[FWD]->getAccumulatedError());

	sdb->PutDouble("TURN rate Drive P Gain",
			m_rate_control[TURN]->getProportionalGain());
	sdb->PutDouble("TURN rate Drive I Gain", m_rate_control[TURN]->getIntegralGain());
	sdb->PutDouble("TURN rate Drive D Gain", m_rate_control[TURN]->getDerivativeGain());
	sdb->PutDouble("TURN rate Drive Error", m_rate_control[TURN]->getError());
	sdb->PutDouble("TURN rate Drive Accumulated Error",
			m_rate_control[TURN]->getAccumulatedError());

}
