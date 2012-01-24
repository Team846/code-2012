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
	 * @brief defaults to gains that do not influence the output
	 * @author Robert Ying
	 * @author Brian Axelrod
	 */
	struct PID_data
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
		float prev_error;
		float acc_error;
	};

	/*!
	 * @brief Constructs a PID controller. All gains are in constant (not timebase) form
	 * @param p_gain proportional gain
	 * @param i_gain integral gain
	 * @param d_gain derivative gain
	 * @param ff_gain feedforward gain (defaults to 1.0)
	 * @param i_decay integral decay (defaults to 0.5)
	 * @param feedforward whether or not feedforward is used
	 */
	PIDController(float p_gain, float i_gain, float d_gain,
			float ff_gain = 1.0, float i_decay = 0.5, bool feedforward = true);

	/*!
	 * @brief Sets PID parameters. All gains are in constant (not timebase) form
	 * @brief This method does reset accumulated error and differential error
	 * @param p_gain proportional gain
	 * @param i_gain integral gain
	 * @param d_gain derivative gain
	 * @param ff_gain feedforward gain (defaults to 1.0)
	 * @param i_decay integral decay (defaults to 0.5)
	 * @param feedforward whether or not feedforward is used
	 */
	void setParameters(float p_gain, float i_gain, float d_gain,
			float ff_gain = 1.0, float i_decay = 0.5, bool feedforward = true);

	/*!
	 * @brief updates the PID controller, call on each loop
	 * @param dt time differential
	 * @return pid output
	 */
	float update(float dt);

	/*!
	 * @brief sets the feedback input
	 * @param PID input
	 */
	void setInput(float input);

	/*!
	 * @brief gets the PID output
	 * @return PID output
	 */
	float getOutput();

	/*!
	 * @brief status flag for feed-forward PID
	 * @return whether this controller uses feedforward PID
	 */
	bool isFeedForward();

	/*!
	 * @brief returns pointer to struct of current PID Data for testing
	 * @return pointer to PID data struct
	 */
	PID_data * getPIDData();

	/*!
	 * @brief resets PID setpoint, input, error, accumulated error, differential, and output
	 */
	void reset();

private:
	PID_data m_data;
	bool m_is_feed_forward;
};
#endif
