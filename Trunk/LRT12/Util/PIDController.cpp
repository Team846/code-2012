#include "PIDController.h"

PIDController::PIDController(float p_gain, float i_gain, float d_gain,
		float ff_gain, float i_decay, bool feedforward)
{
	setParameters(p_gain, i_gain, d_gain, ff_gain, i_decay, feedforward);
}

void PIDController::setParameters(float p_gain, float i_gain, float d_gain,
		float ff_gain, float i_decay, bool feedforward)
{
	reset();
	m_data.proportional_gain = p_gain;
	m_data.integral_gain = i_gain;
	m_data.derivative_gain = d_gain;
	m_data.feedforward_gain = ff_gain;
	m_data.integral_decay = i_decay;
	m_is_feed_forward = feedforward;
}

float PIDController::update(float dt)
{
	m_data.error = m_data.setpoint - m_data.input;

	// calculate discrete derivative
	float delta = (m_data.error - m_data.prev_error) / dt;

	// approximate with riemann sum and decay
	m_data.acc_error *= m_data.integral_decay;
	m_data.acc_error += m_data.error * dt;
	m_data.acc_error *= (1 - m_data.integral_decay);

	// magic PID line
	float PID_output = m_data.proportional_gain * (m_data.error
			+ m_data.integral_gain * m_data.acc_error + m_data.derivative_gain
			* delta);

	if (m_is_feed_forward)
	{
		// feed-forward PID
		m_data.output = m_data.setpoint * m_data.feedforward_gain + PID_output;
	}
	else
	{
		// standard PID
		m_data.output = PID_output;
	}

	m_data.prev_error = m_data.error;

	return m_data.output;
}

void PIDController::setInput(float input)
{
	m_data.input = input;
}

float PIDController::getOutput()
{
	return m_data.output;
}

PIDController::PID_data * PIDController::getPIDData()
{
	return &m_data;
}

void PIDController::reset()
{
	m_data.acc_error = 0.0;
	m_data.error = 0.0;
	m_data.prev_error = 0.0;
	m_data.input = 0.0;
	m_data.output = 0.0;
	m_data.setpoint = 0.0;
}
