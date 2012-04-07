#include "PID.h"
#include "AsyncPrinter.h"

PID::PID(double p_gain, double i_gain, double d_gain, double ff_gain,
		double i_decay, bool feedforward) :
	m_runningSum(0.87)
{
	setParameters(p_gain, i_gain, d_gain, ff_gain, i_decay, feedforward);
	m_IIREnabled = false;
}

PID::PID() :
	m_runningSum(0.87)
{
	setParameters(0, 0, 0);
	m_IIREnabled = false;
}

void PID::setParameters(double p_gain, double i_gain, double d_gain,
		double ff_gain, double i_decay, bool feedforward)
{
	reset();
	m_proportional_gain = p_gain;
	m_integral_gain = i_gain;
	m_derivative_gain = d_gain;
	m_feedforward_gain = ff_gain;
	m_integral_decay = i_decay != 1.0 ? i_decay : 0.9999999999999;
	m_is_feed_forward = feedforward;
	enablePID();
}

double PID::update(double dt)
{
	m_error = m_setpoint - m_input;
	//	AsyncPrinter::Printf("Error: %.02f\r\n", m_error);

	if (m_IIREnabled)
	{
		m_error = m_runningSum.UpdateSum(m_error);
	}
	else
	{
		m_runningSum.Clear();
	}

	// calculate discrete derivative
	double delta = (m_error - m_prev_error) / dt;

	// approximate with riemann sum and decay
	m_acc_error *= m_integral_decay;
	m_acc_error += m_error * dt;
	if (m_acc_error != m_acc_error) // catch NaN
	{
		m_acc_error = 0;
	}
	double integral = m_acc_error / (1 - m_integral_decay);

	// magic PID line
	double PID_output = m_proportional_gain * (m_error + m_integral_gain
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

void PID::setSetpoint(double setpoint)
{
	m_setpoint = setpoint;
}

void PID::setInput(double input)
{
	m_input = input;
}

double PID::getProportionalGain()
{
	return m_proportional_gain;
}

double PID::getIntegralGain()
{
	return m_integral_gain;
}

double PID::getDerivativeGain()
{
	return m_derivative_gain;
}

double PID::getFeedForwardGain()
{
	return m_feedforward_gain;
}

double PID::getIntegralDecay()
{
	return m_integral_decay;
}

double PID::getInput()
{
	return m_input;
}

double PID::getSetpoint()
{
	return m_setpoint;
}

double PID::getError()
{
	return m_error;
}

double PID::getAccumulatedError()
{
	return m_acc_error;
}

double PID::getPreviousError()
{
	return m_prev_error;
}

double PID::getOutput()
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

void PID::setIIREnabled(bool enabled)
{
	m_IIREnabled = enabled;
	if (!enabled)
		m_runningSum.Clear();
}

void PID::setIIRDecay(double decay)
{
	m_runningSum.setDecayConstant(decay);
}
