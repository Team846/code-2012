#include "PIDController.h"

PIDController::PIDController(PID_params * params, bool feedforward)
{
	m_params = params;
	m_acc_error = 0;
	m_prev_error = 0;
	m_is_feed_forward = feedforward;
}

float PIDController::update(float dt)
{
	m_params->error = m_params->setpoint - m_params->input;

	// calculate discrete derivative
	float delta = (m_params->error - m_prev_error) / dt;

	// approximate with riemann sum and decay
	m_acc_error *= m_params->integral_decay;
	m_acc_error += m_params->error * dt;
	m_acc_error *= (1 - m_params->integral_decay);

	// magic PID line
	float PID_output = m_params->proportional_gain * (m_params->error
			+ m_params->integral_gain * m_acc_error + m_params->derivative_gain
			* delta);

	if (m_is_feed_forward)
	{
		// feed-forward PID
		m_params->output = m_params->setpoint * m_params->feedforward_gain
				+ PID_output;
	}
	else
	{
		// standard PID
		m_params->output = PID_output;
	}

	m_prev_error = m_params->error;

	return m_params->output;
}

PIDController::PID_params * PIDController::getPIDParams()
{
	return m_params;
}
