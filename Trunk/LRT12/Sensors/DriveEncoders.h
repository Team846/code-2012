#ifndef DRIVE_ENCODERS_H_
#define DRIVE_ENCODERS_H_

#include "WPILib.h"
#include "LRTEncoder.h"
#include "..\Config\Configurable.h"
#include "..\Config\RobotConfig.h"
#include "..\Util\Util.h"
#include "..\Util\AsynchronousPrinter.h"

#define CHANGEME 0

#define LEFT 0
#define RIGHT 1

class Config; // forward declare config

/*!
 * @brief Class to keep track of drivetrain encoders
 */

class DriveEncoders: public Configurable
{
private:
	static DriveEncoders* m_instance;
	Config& m_config;
	std::string m_configsection;
	LRTEncoder m_encoder_left;
	LRTEncoder m_useless_encoder;
	LRTEncoder m_encoder_right;

	bool m_is_in_high_gear;

public:
	/*!
	 * @brief Gets the singleton instance
	 * @return the instance
	 */
	static DriveEncoders& GetInstance();

	/*!
	 * @brief Destructor
	 */
	virtual ~DriveEncoders();

	const float PULSES_PER_REVOLUTION; // pulses per wheel revolution [BA]

	const float ENCODER_RATE_HIGH_GEAR;
	const float MAX_TURNING_RATE;

	const float TICKS_PER_FULL_TURN;
	const float WHEEL_DIAMETER; // in

	// extrapolate max low gear speed
	const float LOW_GEAR_MULTIPLIER;

	const static float PI = 3.14159;

	/*!
	 * @brief sets the current gear ratio
	 * @param isHighGear
	 */
	void SetHighGear(bool isHighGear);

	/*!
	 * @brief Configures overall settings
	 */
	void Configure();

	/*!
	 * @brief Get non-normalized average forward speed
	 * @return forward speed
	 */
	double RawForwardSpeed(); //not normalized

	/*!
	 * @brief Get normalized (by gear) average forward speed
	 * @return forward speed
	 */
	double GetNormalizedForwardMotorSpeed(); //considers gear

	/*!
	 * @brief Get normalized turning speed assuming low gear
	 * @return turning speed
	 */
	double GetNormalizedLowGearTurningSpeed();

	/*!
	 * @brief Get non-normalized turning speed
	 * @return turning speed
	 */
	double GetTurningSpeed();

	/*!
	 * @brief Get normalized turning speed assuming high gear
	 * @return turning speed
	 */
	double GetNormalizedTurningSpeed();

	/*!
	 * @brief Get normalized turning speed based on current gear
	 * @return turnign speed
	 */
	double GetNormalizedTurningMotorSpeed();

	/*!
	 * @brief Get robot traveled distance in engineering units
	 * @return distance
	 */
	double GetRobotDist();

	/*!
	 * @brief Get number of ticks during turn
	 * @return turn ticks
	 */
	int GetTurnTicks();

	/*!
	 * @brief Get number of revolutions during turn
	 * @return turn revolutions
	 */
	double GetTurnRevolutions();

	/*!
	 * @brief Get calibrated turning angle
	 * @return turn agnle
	 */
	double GetTurnAngle();

	/*!
	 * @brief Get the distance of a particular wheel
	 * @param side LEFT or RIGHT
	 * @return
	 */
	double GetWheelDist(int side);
	//    double GetLeftWheelDist();
	//   double GetRightWheelDist();

	/*!
	 * @brief Get left wheel speed
	 * @return speed
	 */
	double GetLeftSpeed();

	//    double GetNormalizedLeftSpeed();
	//speed of the motor if it were engaged
	//    double GetNormalizedLeftMotorSpeed();

	/*!
	 * @brief Get normalized left encoder speed
	 * @return encoder speed
	 */
	double GetNormalizedLeftOppositeGearMotorSpeed();

	/*!
	 * @brief Get right enocder speed
	 * @return encoder speed
	 */
	double GetRightSpeed();
	//    double GetNormalizedRightSpeed();
	//    double GetNormalizedRightMotorSpeed();

	/*!
	 * @brief Get normalized right encoder speed
	 * @return encoder speed
	 */
	double GetNormalizedRightOppositeGearMotorSpeed();

	/*!
	 * @brief Get the other motor's speed
	 * @param encoder other encoder
	 * @return the normalized speed of the non-active gear
	 */
	double GetNormalizedOpposingGearMotorSpeed(LRTEncoder& encoder);//gets the speed of the gear we are *NOT* in for shifting -BA

	/*!
	 * @brief Normalized motor speed
	 * @param encoder which motor
	 * @return
	 */
	double GetNormalizedMotorSpeed(LRTEncoder& encoder);

	/*!
	 * @brief Determines which gear will be used for calculation
	 * @return whether or not the robot is in high gear
	 */
	bool IsHighGear();

	/*!
	 * @brief The left encoder
	 * @return a reference to the left encoder
	 */
	LRTEncoder& GetLeftEncoder();

	/*!
	 * @brief The right encoder
	 * @return a reference to the right encoder
	 */
	LRTEncoder& GetRightEncoder();

protected:
	DriveEncoders();DISALLOW_COPY_AND_ASSIGN(DriveEncoders);
};

#endif
