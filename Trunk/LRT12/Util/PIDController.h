#ifndef PID_CONTROLLER_H_
#define PID_CONTROLLER_H_

/*!
 * @brief Simple implementation of a PID controller
 * @author Robert Ying
 * @author Brian Axelrod
 */

class PIDController
{
public:
	/*!
	 * @brief standard PID parameters, values are constants, not time bases
	 * @author Robert Ying
	 * @author Brian Axelrod
	 */
	typedef struct
	{
		float proportional_gain;
		float integral_gain;
		float derivative_gain;
		float feedforward_gain;
		float integral_decay;
		float input;
		float output;
		float setpoint;
		float error;
	} PID_params;

	/*!
	 * @brief Constructs a PID controller
	 * @param params pointer to PID_params struct
	 * @param feedforward determines whether or not setpoint is added to output
	 */
	PIDController(PID_params * params, bool feedforward = true);

	/*!
	 * @brief updates the PID controller, call on each loop
	 * @param dt
	 * @return pid output
	 */
	float update(float dt);

	/*!
	 * @return pointer to PID_params struct
	 */
	PID_params * getPIDParams();

private:
	PID_params * m_params;
	float m_acc_error;
	float m_prev_error;
	bool m_is_feed_forward;
};
#endif
