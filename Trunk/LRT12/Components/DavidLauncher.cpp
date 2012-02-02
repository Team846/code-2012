#include "DavidLauncher.h"

DavidLauncher::DavidLauncher() :
	Component(), m_name("Launcher")
{
	m_top_roller = new AsyncCANJaguar(RobotConfig::CAN::ROLLER_TOP,
			"Top Roller");
	m_bottom_roller = new AsyncCANJaguar(RobotConfig::CAN::ROLLER_BOTTOM,
			"Bottom Roller");
	m_prevstate = ACTION::LAUNCHER::DISABLED;
	Configure();
}

DavidLauncher::~DavidLauncher()
{
	delete m_top_roller;
	delete m_bottom_roller;
}

void DavidLauncher::Configure()
{
	Config * c = Config::GetInstance();
	m_pid_top[PROPORTIONAL] = c->Get<double> (m_name, "topRollerP", 1.0);
	m_pid_top[INTEGRAL] = c->Get<double> (m_name, "topRollerI", 0.0);
	m_pid_top[DERIVATIVE] = c->Get<double> (m_name, "topRollerD", 0.0);
	
	m_pid_bottom[PROPORTIONAL] = c->Get<double> (m_name, "topRollerP", 1.0);
	m_pid_bottom[INTEGRAL] = c->Get<double> (m_name, "topRollerI", 0.0);
	m_pid_bottom[DERIVATIVE] = c->Get<double> (m_name, "topRollerD", 0.0);
}

void DavidLauncher::Output()
{
	if (m_prevstate != action->launcher->state)
	{
		m_prevstate = action->launcher->state;
		switch (action->launcher->state)
		{
		case ACTION::LAUNCHER::RUNNING:
			m_top_roller->ChangeControlMode(AsyncCANJaguar::kSpeed);
			m_top_roller->SetSpeedReference(AsyncCANJaguar::kSpeedRef_Encoder);
			m_top_roller->ConfigNeutralMode(AsyncCANJaguar::kNeutralMode_Coast);
			m_top_roller->SetPID(m_pid_top[PROPORTIONAL], m_pid_top[INTEGRAL],
					m_pid_top[DERIVATIVE]);
			m_bottom_roller->ChangeControlMode(AsyncCANJaguar::kSpeed);
			m_bottom_roller->SetSpeedReference(
					AsyncCANJaguar::kSpeedRef_Encoder);
			m_bottom_roller->ConfigNeutralMode(
					AsyncCANJaguar::kNeutralMode_Coast);
			m_bottom_roller->SetPID(m_pid_bottom[PROPORTIONAL],
					m_pid_bottom[INTEGRAL], m_pid_bottom[DERIVATIVE]);
			break;
		case ACTION::LAUNCHER::DISABLED:
			m_top_roller->ChangeControlMode(AsyncCANJaguar::kPercentVbus);
			m_top_roller->ConfigNeutralMode(AsyncCANJaguar::kNeutralMode_Coast);
			m_bottom_roller->ChangeControlMode(AsyncCANJaguar::kPercentVbus);
			m_bottom_roller->ConfigNeutralMode(
					AsyncCANJaguar::kNeutralMode_Coast);
		}
	}
	switch (action->launcher->state)
	{
	case ACTION::LAUNCHER::RUNNING:
		m_top_roller->SetVelocity(action->launcher->topSpeed);
		m_bottom_roller->SetVelocity(action->launcher->bottomSpeed);
		break;
	case ACTION::LAUNCHER::DISABLED:
		m_top_roller->SetDutyCycle(0.0);
		m_bottom_roller->SetDutyCycle(0.0);
		break;

	}
}

std::string DavidLauncher::GetName()
{
	return m_name;
}

void DavidLauncher::log()
{
	SmartDashboard::GetInstance()->PutString(
			"Launcher State",
			(action->launcher->state == ACTION::LAUNCHER::RUNNING) ? "Running"
					: " Disabled");
}