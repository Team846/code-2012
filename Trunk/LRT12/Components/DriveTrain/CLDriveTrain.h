#ifndef CL_RATE_TRAIN_H_
#define CL_RATE_TRAIN_H_

#include "..\..\Sensors\DriveEncoders.h"
#include "..\..\Config\Config.h"
#include "..\..\Config\Configurable.h"
#include "../../Util/PID.h"
#include "../../Log/Loggable.h"

/*!
 * @brief implements a closed-loop drivetrain
 * @brief DOES NOT implement absolute position control
 * @brief DOES implement closed-loop rate control
 * @brief DOES implement closed-loop relative position control
 * @author Robert Ying
 */
class ClosedLoopDrivetrain: public Loggable
{
public:
	/*!
	 * @brief Types of control
	 */
	typedef enum
	{
		CL_DISABLED, CL_RATE, CL_POSITION
	} CONTROL_TYPE;

	typedef struct
	{
		double leftDutyCycle, rightDutyCycle;
		bool shouldLinearize;
	} DriveCommand;

	/*!
	 * @brief constructs the drivetrain
	 */
	ClosedLoopDrivetrain();

	virtual void Configure();

	virtual void log();

private:
	/*!
	 * @brief Drives at a set forward and turn rate
	 * @param rawFwd rate of movement colinear with direction of motion
	 * @param rawTurn rate of rotation about center of mass
	 * @return drive command for escs
	 */
	DriveCommand Drive(double rawFwd, double rawTurn);
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
	CONTROL_TYPE getTranslateMode();

	/*!
	 * @brief Gets the current turn mode
	 * @return current turn mode
	 */
	CONTROL_TYPE getTurnMode();

	/*!
	 * @brief Set drive position relative to current
	 * @param pos relative position
	 */
	void setRelativeTranslatePosition(double pos);
	
	/*!
	 * @brief Set drive position setpoint
	 * @param pos absolute position
	 */
	void setAbsoluteTranslatePosition(double pos);

	/*!
	 * @brief Set current drive position as zero
	 * @param pos relative position
	 */
	void SetCurrentTranslatePositionAsZero();
	

	/*!
	 * Gets current drive setpoint
	 * @return
	 */
	double getTranslateSetpoint();

	/*!
	 * @brief Set drive rate
	 * @param rate speed
	 */
	void setTranslateRate(double rate);

	/*!
	 * @brief Sets raw drive duty cycle
	 * @param duty
	 */
	void setTranslateDriveDutyCycle(double duty);

	/*!
	 * @brief Set turn position relative to current
	 * @param pos
	 */
	void setRelativeTurnPosition(double pos);

	/*!
	 * @brief Set turn rate
	 * @param rate
	 */
	void setTurnRate(double rate);

	/*!
	 * @brief Gets current turn setpoint
	 * @return
	 */
	double getTurnSetpoint();

	/*!
	 * @brief Set raw turn duty cycle
	 * @param duty
	 */
	void setRawTurnDutyCycle(double duty);

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
	void setTranslateControl(CONTROL_TYPE type);

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
	 * @brief gets the gear
	 * @param returns true if high gear is engaged
	 */
	bool getHighGear();

	/*!
	 * @brief Resets PID integration
	 */
	void reset();

private:
	DriveEncoders& m_encoders;

	Config *m_config;

	PID m_rate_drive_high_gear_pid;
	PID m_rate_drive_low_gear_pid;
	PID m_rate_turn_high_gear_pid;
	PID m_rate_turn_low_gear_pid;
	PID m_drive_disabled;
	PID m_pos_drive_high_gear_pid;
	PID m_pos_drive_low_gear_pid;
	PID m_pos_turn_high_gear_pid;
	PID m_pos_turn_low_gear_pid;
	PID m_turn_disabled;

	const static int TURN = 0, TRANSLATE = 1;
	PID *m_pos_control[2];
	PID *m_rate_control[2];
	double output[2];

	bool m_brake_left;
	bool m_brake_right;

	CONTROL_TYPE m_translate_ctrl_type;
	CONTROL_TYPE m_turn_control_type;
	bool m_fwd_op_complete;
	bool m_turn_op_complete;
	double m_translate_zero;

	bool m_in_high_gear;

	const static double FWD_DECAY = 0.87;
	const static double TURN_DECAY = 0.87; // (1/2)^(1/5) =~ 0.87
};

#endif
