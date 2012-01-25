#ifndef PID_CONTROLLER_H_
#define PID_CONTROLLER_H_

/*!
 * @brief Simple implementation of a PID controller
 * @author Robert Ying
 * @author Brian Axelrod
 */

class PID_controller
{
public:
	/*!
	 * @brief Constructs a PID controller. All gains are in constant (not timebase) form
	 * @param p_gain proportional gain
	 * @param i_gain integral gain
	 * @param d_gain derivative gain
	 * @param ff_gain feedforward gain (defaults to 1.0)
	 * @param i_decay integral decay (defaults to 0.5)
	 * @param feedforward whether or not feedforward is used
	 */
	PID_controller(float p_gain, float i_gain, float d_gain,
			float ff_gain = 1.0, float i_decay = 0.5, bool feedforward = true);

	/*!
	 * @brief Constructs a PID controller with no gains set
	 */
	PID_controller();

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
	 * @brief sets the setpoint
	 * @param setpoint
	 */
	void setSetpoint(float setpoint);

	/*!
	 * @brief gets the PID output
	 * @return PID output
	 */
	float getOutput();

	/*!
	 * @brief Gets the p gain
	 * @return proportional PID gain
	 */
	float getProportionalGain();

	/*!
	 * @brief Gets the i gain
	 * @return integral PID gain
	 */
	float getIntegralGain();

	/*!
	 * @brief Gets the d gain
	 * @return derivative PID gain
	 */
	float getDerivativeGain();

	/*!
	 * @brief Gets the feed forward gain
	 * @return feed forward gain
	 */
	float getFeedForwardGain();

	/*!
	 * @brief Gets the integral decay rate
	 * @return integral decay rate
	 */
	float getIntegralDecay();

	/*!
	 * @brief Gets the input value
	 * @return input
	 */
	float getInput();

	/*!
	 * @brief Gets the setpoint value
	 * @return setpoint
	 */
	float getSetpoint();

	/*!
	 * @brief Gets the error value
	 * @return error
	 */
	float getError();

	/*!
	 * @brief Gets the accumulated running sum
	 * @return running sum
	 */
	float getAccumulatedError();

	/*!
	 * @brief Gets the previous error (one iteration ago)
	 * @return previous error
	 */
	float getPreviousError();

	/*!
	 * @brief status flag for feed-forward PID
	 * @return whether this controller uses feedforward PID
	 */
	bool isFeedForward();

	/*!
	 * @brief Resets PID setpoint, input, error, accumulated error, differential, and output
	 */
	void reset();

	/*!
	 * @brief Disables PID -- output returns setpoint
	 */
	void disablePID();

	/*!
	 * @brief Enables PID
	 */
	void enablePID();

private:
	float m_proportional_gain;
	float m_integral_gain;
	float m_derivative_gain;
	float m_feedforward_gain;
	float m_integral_decay;
	float m_input;
	float m_output;
	float m_setpoint;
	float m_error;
	float m_prev_error;
	float m_acc_error;
	bool m_is_feed_forward;
	bool m_enabled;
};
#endif
