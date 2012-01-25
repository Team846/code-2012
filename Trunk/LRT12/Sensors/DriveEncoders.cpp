#include "DriveEncoders.h"
#include "..\Config\Config.h"

DriveEncoders* DriveEncoders::m_instance = NULL;

DriveEncoders& DriveEncoders::GetInstance()
{
	if (m_instance == NULL)
		m_instance = new DriveEncoders();
	return *m_instance;
}

DriveEncoders::DriveEncoders() :
			m_config(Config::GetInstance()),
			m_configsection("DriveEncoders"),
			m_encoder_left(RobotConfig::DIGITAL_IO::ENCODER_LEFT_A,
					RobotConfig::DIGITAL_IO::ENCODER_LEFT_B)
#ifndef VIRTUAL
			,
			m_useless_encoder(3, 6)
#endif
			,
			m_encoder_right(RobotConfig::DIGITAL_IO::ENCODER_RIGHT_A,
					RobotConfig::DIGITAL_IO::ENCODER_RIGHT_B),
			m_is_in_high_gear(false)
{
	// want to stay with ticks/second
	m_encoder_left.SetDistancePerPulse(1);
	m_encoder_right.SetDistancePerPulse(1);

	m_encoder_left.Start();
	m_encoder_right.Start();
	printf("Construct Drive Encoders\n");
}

DriveEncoders::~DriveEncoders()
{
}

void DriveEncoders::Configure()
{
	PULSES_PER_REVOLUTION = m_config.Get<float> (m_configsection,
			"pulses_per_revolution", 100.0);
	ENCODER_RATE_HIGH_GEAR = m_config.Get<float> (m_configsection,
			"high_gear_encoder_rate", 1475.0);
	MAX_TURNING_RATE = m_config.Get<float> (m_configsection,
			"max_turning_rate", 2950.0);
	TICKS_PER_FULL_TURN = m_config.Get<float> (m_configsection,
			"ticks_per_full_turn", 1350.0 * 180.0 / 165.0);
	WHEEL_DIAMETER = m_config.Get<float> (m_configsection, "wheel_diameter",
			4.0); // inches

	// extrapolate max low gear speed
	LOW_GEAR_MULTIPLIER = m_config.Get<float> (m_configsection,
			"low_gear_multiplier", 16.3 / 6.4);
}

double DriveEncoders::RawForwardSpeed()
{
	return (m_encoder_left.GetRate() + m_encoder_right.GetRate()) / 2;
}

double DriveEncoders::NormalizedForwardMotorSpeed()
{
	double forwardSpeed = RawForwardSpeed() / ENCODER_RATE_HIGH_GEAR;

	if (!m_is_in_high_gear)
		forwardSpeed *= LOW_GEAR_MULTIPLIER;

	return forwardSpeed;
}

/***************** Turning Functions ***************************/
double DriveEncoders::GetNormalizedLowGearTurningSpeed()
{
	return GetNormalizedTurningSpeed() * LOW_GEAR_MULTIPLIER;
}

double DriveEncoders::GetTurningSpeed()
{
	// WPILib GetRate error still exists with LRTEncoder?
	return m_encoder_right.GetRate() - m_encoder_left.GetRate();
}

double DriveEncoders::GetNormalizedTurningSpeed()
{
	return GetTurningSpeed() / MAX_TURNING_RATE;
}

double DriveEncoders::GetNormalizedTurningMotorSpeed()
{
	return m_is_in_high_gear ? GetNormalizedTurningSpeed()
			: GetNormalizedLowGearTurningSpeed();
}

double DriveEncoders::GetRobotDist()
{
	return (GetWheelDist(LEFT) + GetWheelDist(RIGHT)) / 2;
}

int DriveEncoders::GetTurnTicks()
{
	// CCW is positive, CW is negative
	return m_encoder_right.Get() - m_encoder_left.Get();
}

double DriveEncoders::GetTurnRevolutions()
{
	return GetTurnTicks() / TICKS_PER_FULL_TURN;
}

double DriveEncoders::GetTurnAngle()
{
	return GetTurnRevolutions() * 360.0;
}

/************* Distance functions **************************************/
double DriveEncoders::GetWheelDist(int side)
{
	// pulses / ( pulses / revolution ) * distance / revolution = inch distance
	LRTEncoder& e = (side == LEFT ? m_encoder_left : m_encoder_right);
	return e.Get() / PULSES_PER_REVOLUTION * WHEEL_DIAMETER * PI;
}

double DriveEncoders::GetLeftSpeed()
{
	return m_encoder_left.GetRate();
}

double DriveEncoders::GetNormalizedLeftOppositeGearMotorSpeed()
{
	return Util::Clamp<double>(
			m_encoder_left.GetRate()
					/ (!m_is_in_high_gear ? ENCODER_RATE_HIGH_GEAR
							: (ENCODER_RATE_HIGH_GEAR / LOW_GEAR_MULTIPLIER)),
			-1.0, 1.0);
}

double DriveEncoders::GetRightSpeed()
{
	return m_encoder_right.GetRate();
}

double DriveEncoders::GetNormalizedMotorSpeed(LRTEncoder& encoder)
{
	return encoder.GetRate() / (m_is_in_high_gear ? ENCODER_RATE_HIGH_GEAR
			: (ENCODER_RATE_HIGH_GEAR / LOW_GEAR_MULTIPLIER));
}

double DriveEncoders::GetNormalizedOpposingGearMotorSpeed(LRTEncoder& encoder)
{
	return encoder.GetRate() / (m_is_in_high_gear ? (ENCODER_RATE_HIGH_GEAR
			/ LOW_GEAR_MULTIPLIER) : ENCODER_RATE_HIGH_GEAR);
}

double DriveEncoders::GetNormalizedRightOppositeGearMotorSpeed()
{
	return m_encoder_right.GetRate()
			/ (m_is_in_high_gear ? (ENCODER_RATE_HIGH_GEAR
					/ LOW_GEAR_MULTIPLIER) : ENCODER_RATE_HIGH_GEAR);
}

void DriveEncoders::SetHighGear(bool isHighGear)
{
	this->m_is_in_high_gear = isHighGear;
}

bool DriveEncoders::IsHighGear()
{
	return m_is_in_high_gear;
}

LRTEncoder& DriveEncoders::GetLeftEncoder()
{
	return m_encoder_left;
}

LRTEncoder& DriveEncoders::GetRightEncoder()
{
	return m_encoder_right;
}
