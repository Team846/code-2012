#ifndef CL_RATE_TRAIN_H_
#define CL_RATE_TRAIN_H_

#include "..\..\Sensors\DriveEncoders.h"
#include "..\..\Config\Config.h"
#include "..\..\Config\Configurable.h"
#include "../../Util/PID_controller.h"
#include "DitheredBrakeTrain.h"

/*!
 * @brief implements a closed-loop drivetrain
 * @brief DOES NOT implement absolute position control
 * @brief DOES implement closed-loop rate control
 * @brief DOES implement closed-loop relative position control
 * @author Robert Ying
 */
class ClosedLoopDrivetrain: public DitheredBrakeTrain
{
public:
	/*!
	 * @brief Types of control
	 */
	typedef enum
	{
		CL_DISABLED, CL_RATE, CL_POSITION
	} CONTROL_TYPE;

	/*!
	 * @brief constructs the drivetrain
	 */
	ClosedLoopDrivetrain();

	virtual void Configure();

private:
	/*!
	 * @brief Drives at a set forward and turn rate
	 * @param rawFwd rate of movement colinear with direction of motion
	 * @param rawTurn rate of rotation about center of mass
	 * @return drive command for escs
	 */
	DriveCommand Drive(float rawFwd, float rawTurn);
public:
	/*!
	 * @brief updates loops; run on each cycle
	 */
	void update();

	/*!
	 * @brief Gets the output drive command
	 * @return drive command for escs
	 */
	DriveCommand getOutput();

	/*!
	 * @brief Gets the current drive mode
	 * @return current drive mode
	 */
	CONTROL_TYPE getDriveMode();

	/*!
	 * @brief Gets the current turn mode
	 * @return current turn mode
	 */
	CONTROL_TYPE getTurnMode();

	/*!
	 * @brief Set drive position relative to current
	 * @param pos relative position
	 */
	void setRelativeDrivePosition(float pos);

	/*!
	 * @brief Set drive rate
	 * @param rate speed
	 */
	void setDriveRate(float rate);

	/*!
	 * @brief Sets raw drive duty cycle
	 * @param duty
	 */
	void setRawDriveDutyCycle(float duty);

	/*!
	 * @brief Set turn position relative to current
	 * @param pos
	 */
	void setRelativeTurnPosition(float pos);

	/*!
	 * @brief Set turn rate
	 * @param rate
	 */
	void setTurnRate(float rate);

	/*!
	 * @brief Set raw turn duty cycle
	 * @param duty
	 */
	void setRawTurnDutyCycle(float duty);

	/*!
	 * @brief Returns true when drive operation is complete
	 * @return operation status
	 */
	bool driveOperationComplete();

	/*!
	 * @brief Returns true when turn operation is complete
	 * @return operation status
	 */
	bool turnOperationComplete();

	/*!
	 * @brief Sets type of control for translational motion
	 * @param type control type
	 */
	void setDriveControl(CONTROL_TYPE type);

	/*!
	 * @brief Sets type of control for rotational motion
	 * @param type control type
	 */
	void setTurnControl(CONTROL_TYPE type);

	/*!
	 * @brief sets the gear
	 * @param isHighGear
	 */
	void setHighGear(bool isHighGear);

	/*!
	 * @brief Resets PID integration
	 */
	void reset();

private:
	DriveEncoders& m_encoders;

	Config& m_config;

	PID_controller m_rate_drive_high_gear_pid;
	PID_controller m_rate_drive_low_gear_pid;
	PID_controller m_rate_turn_high_gear_pid;
	PID_controller m_rate_turn_low_gear_pid;
	PID_controller m_drive_disabled;
	PID_controller m_pos_drive_high_gear_pid;
	PID_controller m_pos_drive_low_gear_pid;
	PID_controller m_pos_turn_high_gear_pid;
	PID_controller m_pos_turn_low_gear_pid;
	PID_controller m_turn_disabled;

	PID_controller * m_turn_control;
	PID_controller * m_drive_control;

	bool m_brake_left;
	bool m_brake_right;

	CONTROL_TYPE m_drive_control_type;
	CONTROL_TYPE m_turn_control_type;
	bool m_drive_op_complete;
	bool m_turn_op_complete;

	bool m_in_high_gear;

	const static float FWD_DECAY = 0.87;
	const static float TURN_DECAY = 0.87; // (1/2)^(1/5) =~ 0.87
};

#endif
