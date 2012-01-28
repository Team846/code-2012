#include "BLauncher.h"

BLauncher::BLauncher() :
	Component(), m_name("BLauncher")
{
	m_top_roller = new AsyncCANJaguar(RobotConfig::CAN::ROLLER_TOP,
			"Top Roller");
	m_bottom_roller = new AsyncCANJaguar(RobotConfig::CAN::ROLLER_BOTTOM,
			"Bottom Roller");
	m_prevstate = ACTION::LAUNCHER::DISABLED;
	Configure();
}

BLauncher::~BLauncher()
{
	delete m_top_roller;
	delete m_bottom_roller;
}

void BLauncher::Configure()
{
	Config * c = &Config::GetInstance();
	m_pid_top[proportional] = c->Get<double> (m_name, "topRollerP", 1.0);
	m_pid_top[integral] = c->Get<double> (m_name, "topRollerI", 0.0);
	m_pid_top[derivative] = c->Get<double> (m_name, "topRollerD", 0.0);

	m_pid_bottom[proportional] = c->Get<double> (m_name, "bottomRollerP", 1.0);
	m_pid_bottom[integral] = c->Get<double> (m_name, "bottomRollerI", 0.0);
	m_pid_bottom[derivative] = c->Get<double> (m_name, "bottomRollerD", 0.0);
}
void BLauncher::Output()
{
	if (m_prevstate != action->launcher->state)
	{
		m_prevstate = action->launcher->state;
		if (action->launcher->state == ACTION::LAUNCHER::RUNNING)
		{
			m_top_roller->ChangeControlMode(AsyncCANJaguar::kSpeed);
			m_top_roller->SetSpeedReference(AsyncCANJaguar::kSpeedRef_Encoder);
			m_top_roller->ConfigNeutralMode(AsyncCANJaguar::kNeutralMode_Coast);
			m_top_roller->SetPID(m_pid_top[proportional], m_pid_top[integral],
					m_pid_top[derivative]);
			m_bottom_roller->ChangeControlMode(AsyncCANJaguar::kSpeed);
			m_bottom_roller->SetSpeedReference(
					AsyncCANJaguar::kSpeedRef_Encoder);
			m_bottom_roller->ConfigNeutralMode(
					AsyncCANJaguar::kNeutralMode_Coast);
			m_bottom_roller->SetPID(m_pid_bottom[proportional],
					m_pid_bottom[integral], m_pid_bottom[derivative]);
		}
		else if (action->launcher->state == ACTION::LAUNCHER::DISABLED)
		{
			m_top_roller->ChangeControlMode(AsyncCANJaguar::kPercentVbus);
			m_top_roller->ConfigNeutralMode(AsyncCANJaguar::kNeutralMode_Coast);
			m_bottom_roller->ChangeControlMode(AsyncCANJaguar::kPercentVbus);
			m_bottom_roller->ConfigNeutralMode(
					AsyncCANJaguar::kNeutralMode_Coast);

		}
	}
	if (action->launcher->state == ACTION::LAUNCHER::RUNNING)
	{
		m_top_roller->SetVelocity(action->launcher->topSpeed);
		m_bottom_roller->SetVelocity(action->launcher->bottomSpeed);
	}
	else if (action->launcher->state == ACTION::LAUNCHER::DISABLED)
	{
		m_top_roller->SetDutyCycle(0.0);
		m_bottom_roller->SetDutyCycle(0.0);
	}
}
std::string BLauncher::GetName()
{
	return m_name;
}
void BLauncher::log()
{
	SmartDashboard::GetInstance()->PutString(
			"Launcher State",
			(action->launcher->state == ACTION::LAUNCHER::RUNNING ? "Running"
					: "Disabled"));
}
