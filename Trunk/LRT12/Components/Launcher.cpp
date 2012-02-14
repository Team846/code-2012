#include "Launcher.h"

Launcher::Launcher() :
	Component(), m_name("Launcher")
{
	m_roller = new AsyncCANJaguar(RobotConfig::CAN::LAUNCHER, "Roller");
	m_enc = new Counter((UINT32) RobotConfig::DIGITAL_IO::HALL_EFFECT);

	m_enc->Start();
	m_enc->SetMaxPeriod(0.1); // consider stopped if slower than 10rpm
	Configure();
}

Launcher::~Launcher()
{
	delete m_enc;
	delete m_roller;
}

void Launcher::Configure()
{
	Config * c = Config::GetInstance();
	double p = c->Get<double> (m_name, "rollerP", 1.0);
	double i = c->Get<double> (m_name, "rollerI", 0.0);
	double d = c->Get<double> (m_name, "rollerD", 0.0);
	double ff = c->Get<double> (m_name, "rollerFF", 1.0);

	m_pid.setParameters(p, i, d, ff);
}

void Launcher::Output()
{
	switch (action->launcher->state)
	{
	case ACTION::LAUNCHER::RUNNING:
		m_pid.setSetpoint(action->launcher->speed);

		double speed = (m_enc->GetStopped()) ? 0.0 : (1.0 / m_enc->GetPeriod());
		m_pid.setInput(speed);
		m_pid.update(1.0 / 50); // 50 Hz
		m_roller->SetDutyCycle(m_pid.getOutput());
		action->launcher->atSpeed = fabs(speed - action->launcher->speed) < 10; // w/i 10 rpm
		break;
	case ACTION::LAUNCHER::DISABLED:
		action->launcher->atSpeed = false;
		m_roller->SetDutyCycle(0.0);
		m_pid.reset();
		break;
	}
}

std::string Launcher::GetName()
{
	return m_name;
}

void Launcher::log()
{
	SmartDashboard::GetInstance()->PutString(
			"Launcher State",
			(action->launcher->state == ACTION::LAUNCHER::RUNNING) ? "Running"
					: "Disabled");
}
