#include "PID.h"

PID::PID(float p_gain, float i_gain, float d_gain, float ff_gain,
		float i_decay, bool feedforward)
{
	setParameters(p_gain, i_gain, d_gain, ff_gain, i_decay, feedforward);
}

PID::PID()
{
	setParameters(0, 0, 0);
}

void PID::setParameters(float p_gain, float i_gain, float d_gain,
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

float PID::update(float dt)
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

void PID::setSetpoint(float setpoint)
{
	m_setpoint = setpoint;
}

void PID::setInput(float input)
{
	m_input = input;
}

float PID::getProportionalGain()
{
	return m_proportional_gain;
}

float PID::getIntegralGain()
{
	return m_integral_gain;
}

float PID::getDerivativeGain()
{
	return m_derivative_gain;
}

float PID::getFeedForwardGain()
{
	return m_feedforward_gain;
}

float PID::getIntegralDecay()
{
	return m_integral_decay;
}

float PID::getInput()
{
	return m_input;
}

float PID::getSetpoint()
{
	return m_setpoint;
}

float PID::getError()
{
	return m_error;
}

float PID::getAccumulatedError()
{
	return m_acc_error;
}

float PID::getPreviousError()
{
	return m_prev_error;
}

float PID::getOutput()
{
	if (m_enabled)
		return m_output;
	else
		return m_setpoint;
}

void PID::disablePID()
{
	m_enabled = false;
}

void PID::enablePID()
{
	m_enabled = true;
}

void PID::reset()
{
	m_acc_error = 0.0;
	m_error = 0.0;
	m_prev_error = 0.0;
	m_input = 0.0;
	m_output = 0.0;
	m_setpoint = 0.0;
}
