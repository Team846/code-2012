#include "DriveEncoders.h"
#include "..\Config\config.h"

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
			m_encoder_left("Left Drive Encoder",
					RobotConfig::DIGITAL_IO::ENCODER_LEFT_A,
					RobotConfig::DIGITAL_IO::ENCODER_LEFT_B),
			m_useless_encoder("Useless Encoder", 3, 6),
			m_encoder_right("Right Drive Encoder",
					RobotConfig::DIGITAL_IO::ENCODER_RIGHT_A,
					RobotConfig::DIGITAL_IO::ENCODER_RIGHT_B),
			PULSES_PER_REVOLUTION(
					m_config->Get<double> (m_configsection,
							"pulses_per_revolution", 100.0)),
			ENCODER_RATE_HIGH_GEAR(
					m_config->Get<double> (m_configsection,
							"high_gear_encoder_rate", 1475.0)),
			MAX_TURNING_RATE(
					m_config->Get<double> (m_configsection, "max_turning_rate",
							2950.0)),
			TICKS_PER_FULL_TURN(
					m_config->Get<double> (m_configsection,
							"ticks_per_full_turn", 1350.0 * 180.0 / 165.0)),
			WHEEL_DIAMETER(
					m_config->Get<double> (m_configsection, "wheel_diameter",
							4.0)), // inches
			LOW_GEAR_MULTIPLIER(
					m_config->Get<double> (m_configsection,
							"low_gear_multiplier", 16.3 / 6.4))

{
	m_is_in_high_gear = false;
	m_useless_encoder.disableLog();
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
	m_encoder_left.Start();
	m_encoder_right.Start();
}

double DriveEncoders::rawForwardSpeed()
{
	return (m_encoder_left.GetRate() + m_encoder_right.GetRate()) / 2;
}

double DriveEncoders::getNormalizedForwardMotorSpeed()
{
	double forwardSpeed = rawForwardSpeed() / ENCODER_RATE_HIGH_GEAR;

	if (!m_is_in_high_gear)
		forwardSpeed *= LOW_GEAR_MULTIPLIER;

	return forwardSpeed;
}

/***************** Turning Functions ***************************/
double DriveEncoders::getNormalizedLowGearTurningSpeed()
{
	return getNormalizedTurningSpeed() * LOW_GEAR_MULTIPLIER;
}

double DriveEncoders::getTurningSpeed()
{
	// WPILib GetRate error still exists with LRTEncoder?
	return m_encoder_right.GetRate() - m_encoder_left.GetRate();
}

double DriveEncoders::getNormalizedTurningSpeed()
{
	return getTurningSpeed() / MAX_TURNING_RATE;
}

double DriveEncoders::getNormalizedTurningMotorSpeed()
{
	return m_is_in_high_gear ? getNormalizedTurningSpeed()
			: getNormalizedLowGearTurningSpeed();
}

double DriveEncoders::getRobotDist()
{
	return (getWheelDist(LEFT) + getWheelDist(RIGHT)) / 2;
}

int DriveEncoders::getTurnTicks()
{
	// CCW is positive, CW is negative
	return m_encoder_right.Get() - m_encoder_left.Get();
}

double DriveEncoders::getTurnRevolutions()
{
	return getTurnTicks() / TICKS_PER_FULL_TURN;
}

double DriveEncoders::getTurnAngle()
{
	return getTurnRevolutions() * 360.0;
}

/************* Distance functions **************************************/
double DriveEncoders::getWheelDist(int side)
{
	// pulses / ( pulses / revolution ) * distance / revolution = inch distance
	LRTEncoder& e = (side == LEFT ? m_encoder_left : m_encoder_right);
	double rate = (double) ((e.Get() * 1.0) / PULSES_PER_REVOLUTION
			* WHEEL_DIAMETER * PI);
	return rate;
}

double DriveEncoders::getLeftSpeed()
{
	return m_encoder_left.GetRate();
}

double DriveEncoders::getNormalizedLeftOppositeGearMotorSpeed()
{
	return Util::Clamp<double>(
			m_encoder_left.GetRate()
					/ (!m_is_in_high_gear ? ENCODER_RATE_HIGH_GEAR
							: (ENCODER_RATE_HIGH_GEAR / LOW_GEAR_MULTIPLIER)),
			-1.0, 1.0);
}

double DriveEncoders::getRightSpeed()
{
	return m_encoder_right.GetRate();
}

double DriveEncoders::getNormalizedMotorSpeed(LRTEncoder& encoder)
{
	return encoder.GetRate() / (m_is_in_high_gear ? ENCODER_RATE_HIGH_GEAR
			: (ENCODER_RATE_HIGH_GEAR / LOW_GEAR_MULTIPLIER));
}

double DriveEncoders::getNormalizedOpposingGearMotorSpeed(LRTEncoder& encoder)
{
	return encoder.GetRate() / (m_is_in_high_gear ? (ENCODER_RATE_HIGH_GEAR
			/ LOW_GEAR_MULTIPLIER) : ENCODER_RATE_HIGH_GEAR);
}

double DriveEncoders::getNormalizedRightOppositeGearMotorSpeed()
{
	return m_encoder_right.GetRate()
			/ (m_is_in_high_gear ? (ENCODER_RATE_HIGH_GEAR
					/ LOW_GEAR_MULTIPLIER) : ENCODER_RATE_HIGH_GEAR);
}

void DriveEncoders::setHighGear(bool isHighGear)
{
	this->m_is_in_high_gear = isHighGear;
}

bool DriveEncoders::isHighGear()
{
	return m_is_in_high_gear;
}

LRTEncoder& DriveEncoders::getLeftEncoder()
{
	return m_encoder_left;
}

LRTEncoder& DriveEncoders::getRightEncoder()
{
	return m_encoder_right;
}

void DriveEncoders::log()
{
	SmartDashboard * sdb = SmartDashboard::GetInstance();
	sdb->PutDouble("Robot Drive Speed", getNormalizedForwardMotorSpeed());
	sdb->PutDouble("Robot Turning Speed", getNormalizedTurningMotorSpeed());
	sdb->PutDouble("Robot Drive Distance", getRobotDist());
	sdb->PutDouble("Robot Turn Angle", getTurnAngle());
}
