#ifndef PID_CONTROLLER_H_
#define PID_CONTROLLER_H_

#include "RunningSum.h"
/*!
 * @brief Simple implementation of a PID controller
 * @author Robert Ying
 * @author Brian Axelrod
 */

class PID
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
	PID(double p_gain, double i_gain, double d_gain, double ff_gain = 1.0,
			double i_decay = 0.5, bool feedforward = true);

	/*!
	 * @brief Constructs a PID controller with no gains set
	 */
	PID();

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
	void
			setParameters(double p_gain, double i_gain, double d_gain,
					double ff_gain = 1.0, double i_decay = 0.87,
					bool feedforward = true);

	/*!
	 * @brief updates the PID controller, call on each loop
	 * @param dt time differential
	 * @return pid output
	 */
	double update(double dt);

	/*!
	 * @brief sets the feedback input
	 * @param PID input
	 */
	void setInput(double input);

	/*!
	 * @brief sets the setpoint
	 * @param setpoint
	 */
	void setSetpoint(double setpoint);

	/*!
	 * @brief gets the PID output
	 * @return PID output
	 */
	double getOutput();

	/*!
	 * @brief Gets the p gain
	 * @return proportional PID gain
	 */
	double getProportionalGain();

	/*!
	 * @brief Gets the i gain
	 * @return integral PID gain
	 */
	double getIntegralGain();

	/*!
	 * @brief Gets the d gain
	 * @return derivative PID gain
	 */
	double getDerivativeGain();

	/*!
	 * @brief Gets the feed forward gain
	 * @return feed forward gain
	 */
	double getFeedForwardGain();

	/*!
	 * @brief Gets the integral decay rate
	 * @return integral decay rate
	 */
	double getIntegralDecay();

	/*!
	 * @brief Gets the input value
	 * @return input
	 */
	double getInput();

	/*!
	 * @brief Gets the setpoint value
	 * @return setpoint
	 */
	double getSetpoint();

	/*!
	 * @brief Gets the error value
	 * @return error
	 */
	double getError();

	/*!
	 * @brief Gets the accumulated m_is_running sum
	 * @return m_is_running sum
	 */
	double getAccumulatedError();

	/*!
	 * @brief Gets the previous error (one iteration ago)
	 * @return previous error
	 */
	double getPreviousError();

	/*!
	 * @brief m_status flag for feed-forward PID
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
	
	/*!
	 * @brief Enables an IIR filter on output
	 */
	void setIIREnabled(bool enabled);
	
	/*!
	 * @brief Sets the IFR Decay rate
	 */
	void setIIRDecay(double decay);
	

private:
	double m_proportional_gain;
	double m_integral_gain;
	double m_derivative_gain;
	double m_feedforward_gain;
	double m_integral_decay;
	double m_input;
	double m_output;
	double m_setpoint;
	double m_error;
	double m_prev_error;
	double m_acc_error;
	bool m_is_feed_forward;
	bool m_enabled;
	
	bool m_IIREnabled;
	RunningSum m_runningSum;
};
#endif
