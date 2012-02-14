#ifndef DRIVE_ENCODERS_H_
#define DRIVE_ENCODERS_H_

#include "WPILib.h"
#include "LRTEncoder.h"
#include "..\Config\Configurable.h"
#include "..\Config\RobotConfig.h"
#include "..\Util\Util.h"
#include "../Util/AsyncPrinter.h"
#include "../Log/Loggable.h"

#define CHANGEME 0

#define LEFT 0
#define RIGHT 1

class Config; // forward declare config

/*!
 * @brief Class to keep track of drivetrain encoders
 */

class DriveEncoders: public Configurable, public Loggable
{
private:
	static DriveEncoders* m_instance;
	Config *m_config;
	std::string m_configsection;

	LRTEncoder * m_encoder_left;
	//	LRTEncoder m_useless_encoder;
	LRTEncoder * m_encoder_right;

	bool m_is_in_high_gear;

	double PULSES_PER_REVOLUTION; // pulses per wheel revolution [BA]

	double ENCODER_RATE_HIGH_GEAR;
	double MAX_TURNING_RATE;

	double TICKS_PER_FULL_TURN;
	double WHEEL_DIAMETER; // in

	// extrapolate max low gear speed
	double HIGH_GEAR_MULTIPLIER;

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

	const static double PI = 3.14159;

	double getMaxEncoderRate();
	/*!
	 * @brief sets the current gear ratio
	 * @param isHighGear
	 */
	void setHighGear(bool isHighGear);

	/*!
	 * @brief Configures overall settings
	 */
	void Configure();

	void log();

	/*!
	 * @brief Get non-normalized average forward speed
	 * @return forward speed
	 */
	double rawForwardSpeed(); //not normalized

	/*!
	 * @brief Get normalized (by gear) average forward speed
	 * @return forward speed
	 */
	double getNormalizedForwardMotorSpeed(); //considers gear

	/*!
	 * @brief Get normalized turning speed assuming low gear
	 * @return turning speed
	 */
	double getNormalizedLowGearTurningSpeed();

	/*!
	 * @brief Get non-normalized turning speed
	 * @return turning speed
	 */
	double getTurningSpeed();

	/*!
	 * @brief Get normalized turning speed assuming high gear
	 * @return turning speed
	 */
	double getNormalizedTurningSpeed();

	/*!
	 * @brief Get normalized turning speed based on current gear
	 * @return turnign speed
	 */
	double getNormalizedTurningMotorSpeed();

	/*!
	 * @brief Get robot traveled distance in engineering units
	 * @return distance
	 */
	double getRobotDist();

	/*!
	 * @brief Get number of ticks during turn
	 * @return turn ticks
	 */
	int getTurnTicks();

	/*!
	 * @brief Get number of revolutions during turn
	 * @return turn revolutions
	 */
	double getTurnRevolutions();

	/*!
	 * @brief Get calibrated turning angle
	 * @return turn agnle
	 */
	double getTurnAngle();

	/*!
	 * @brief Get the distance of a particular wheel
	 * @param side LEFT or RIGHT
	 * @return
	 */
	double getWheelDist(int side);
	//    double GetLeftWheelDist();
	//   double GetRightWheelDist();

	/*!
	 * @brief Get left wheel speed
	 * @return speed
	 */
	double getLeftSpeed();

	//    double GetNormalizedLeftSpeed();
	//speed of the motor if it were engaged
	//    double GetNormalizedLeftMotorSpeed();

	/*!
	 * @brief Get normalized left encoder speed
	 * @return encoder speed
	 */
	double getNormalizedLeftOppositeGearMotorSpeed();

	/*!
	 * @brief Get right enocder speed
	 * @return encoder speed
	 */
	double getRightSpeed();
	//    double GetNormalizedRightSpeed();
	//    double GetNormalizedRightMotorSpeed();

	/*!
	 * @brief Get normalized right encoder speed
	 * @return encoder speed
	 */
	double getNormalizedRightOppositeGearMotorSpeed();

	/*!
	 * @brief Get the other motor's speed
	 * @param encoder other encoder
	 * @return the normalized speed of the non-active gear
	 */
	double getNormalizedOpposingGearMotorSpeed(LRTEncoder * encoder);//gets the speed of the gear we are *NOT* in for shifting -BA

	/*!
	 * @brief Normalized motor speed
	 * @param encoder which motor
	 * @return
	 */
	double getNormalizedMotorSpeed(LRTEncoder * encoder);

	/*!
	 * @brief Determines which gear will be used for calculation
	 * @return whether or not the robot is in high gear
	 */
	bool isHighGear();

	/*!
	 * @brief The left encoder
	 * @return a pointer to the left encoder
	 */
	LRTEncoder * getLeftEncoder();

	/*!
	 * @brief The right encoder
	 * @return a pointer to the right encoder
	 */
	LRTEncoder * getRightEncoder();

protected:
	DriveEncoders();DISALLOW_COPY_AND_ASSIGN(DriveEncoders);
};

#endif
