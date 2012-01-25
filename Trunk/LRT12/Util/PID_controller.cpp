#include "PID_controller.h"

PID_controller::PID_controller(float p_gain, float i_gain, float d_gain,
		float ff_gain, float i_decay, bool feedforward)
{
	setParameters(p_gain, i_gain, d_gain, ff_gain, i_decay, feedforward);
}

PID_controller::PID_controller()
{
	setParameters(0, 0, 0);
}

void PID_controller::setParameters(float p_gain, float i_gain, float d_gain,
		float ff_gain, float i_decay, bool feedforward)
{
	reset();
	m_proportional_gain = p_gain;
	m_integral_gain = i_gain;
	m_derivative_gain = d_gain;
	m_feedforward_gain = ff_gain;
	m_integral_decay = i_decay;
	m_is_feed_forward = feedforward;
	enablePID();
}

float PID_controller::update(float dt)
{
	m_error = m_setpoint - m_input;

	// calculate discrete derivative
	float delta = (m_error - m_prev_error) / dt;

	// approximate with riemann sum and decay
	m_acc_error *= m_integral_decay;
	m_acc_error += m_error * dt;
	float integral = m_acc_error / (1 - m_integral_decay);

	// magic PID line
	float PID_output = m_proportional_gain * (m_error + m_integral_gain
			* integral + m_derivative_gain * delta);

	if (m_is_feed_forward)
	{
		// feed-forward PID
		m_output = m_setpoint * m_feedforward_gain + PID_output;
	}
	else
	{
		// standard PID
		m_output = PID_output;
	}

	m_prev_error = m_error;

	return m_output;
}

void PID_controller::setSetpoint(float setpoint)
{
	m_setpoint = setpoint;
}

void PID_controller::setInput(float input)
{
	m_input = input;
}

float PID_controller::getProportionalGain()
{
	return m_proportional_gain;
}

float PID_controller::getIntegralGain()
{
	return m_integral_gain;
}

float PID_controller::getDerivativeGain()
{
	return m_derivative_gain;
}

float PID_controller::getFeedForwardGain()
{
	return m_feedforward_gain;
}

float PID_controller::getIntegralDecay()
{
	return m_integral_decay;
}

float PID_controller::getInput()
{
	return m_input;
}

float PID_controller::getSetpoint()
{
	return m_setpoint;
}

float PID_controller::getError()
{
	return m_error;
}

float PID_controller::getAccumulatedError()
{
	return m_acc_error;
}

float PID_controller::getPreviousError()
{
	return m_prev_error;
}

float PID_controller::getOutput()
{
	if (m_enabled)
		return m_output;
	else
		return m_setpoint;
}

void PID_controller::disablePID()
{
	m_enabled = false;
}

void PID_controller::enablePID()
{
	m_enabled = true;
}

void PID_controller::reset()
{
	m_acc_error = 0.0;
	m_error = 0.0;
	m_prev_error = 0.0;
	m_input = 0.0;
	m_output = 0.0;
	m_setpoint = 0.0;
}
