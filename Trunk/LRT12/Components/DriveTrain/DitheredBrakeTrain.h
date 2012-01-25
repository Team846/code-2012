#ifndef DITHERED_BRAKE_DRIVE
#define DITHERED_BRAKE_DRIVE

#include "..\..\Sensors\DriveEncoders.h"
#include "..\..\Config\Config.h"
#include "..\..\Config\Configurable.h"

/*!
 * @brief Holds the duty cycle and braking duty cycle for a motor
 */
typedef struct
{
	float dutyCycle;
	float brakingDutyCycle;
} ESCCommand;

/*!
 * @brief Holds the overall drivetrain duty cycle commands
 */
typedef struct
{
	ESCCommand leftCommand;
	ESCCommand rightCommand;
} DriveCommand;

/*!
 * @brief Class that linearizes drivetrain commands
 */
class DitheredBrakeTrain: public Configurable
{
private:
	LRTEncoder& leftEncoder;
	LRTEncoder& rightEncoder;

public:
	/*!
	 * @brief Constructs a dithered brake train
	 */
	DitheredBrakeTrain();

	/*!
	 * @brief Destructs a dithered brake train
	 */
	~DitheredBrakeTrain();

	virtual void Configure();

	/*!
	 * @brief Returns motor controller commands that linearize drivetrain response
	 * @param forwardInput translational speed colinear with robot direction of motion
	 * @param turnInput rotational speed around robot center of mass
	 * @return drivetrain duty cycles and braking duty cycles
	 */
	DriveCommand Drive(float forwardInput, float turnInput);

	/*!
	 * @brief Returns a single motor controller's commands based on its desired speed 
	 * @brief and current speed
	 * @param desired_speed desired speed, normalized
	 * @param current_speed current speed, normalized
	 * @return
	 */
	ESCCommand CalculateBrakeAndDutyCycle(float desired_speed,
			float current_speed);
};

#endif
