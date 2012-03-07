#include "Launcher.h"
#include "Pneumatic/Pneumatics.h"

Launcher::Launcher() :
	Component(), m_name("Launcher")
{
	m_roller = new AsyncCANJaguar(RobotConfig::CAN::LAUNCHER, "Roller");
	m_enc = new Counter((UINT32) RobotConfig::DIGITAL_IO::HALL_EFFECT);

	started = false;

	m_enc->Start();
	m_enc->SetMaxPeriod(0.1); // consider stopped if slower than 10rpm
	Configure();

	m_output = 0.0;
	m_duty_cycle_delta = 0.55;
	m_speed = 0;
	m_speed_threshold = 10;
	disableLog();
}

Launcher::~Launcher()
{
	delete m_enc;
	delete m_roller;
}

void Launcher::Configure()
{
	Config * c = Config::GetInstance();
	double p = c->Get<double> (m_name, "rollerP", 250.0);
	double i = c->Get<double> (m_name, "rollerI", 0.0);
	double d = c->Get<double> (m_name, "rollerD", 0.0);
	double ff = c->Get<double> (m_name, "rollerFF", 1);

	m_speed_threshold = c->Get<double> (m_name, "speedThreshold", 10);
	m_max_speed = c->Get<double> (m_name, "maxSpeed", 5180);
	m_action->launcher->speed = c->Get<double> (m_name, "targetSpeed", 3000);
	m_atSpeedCycles = c->Get<int> (m_name, "cyclesAtSpeed", 1);

	m_speeds[SLOW] = c->Get<double> (m_name, "lowSpeed", 2000);
	m_speeds[MEDIUM] = c->Get<double> (m_name, "mediumSpeed", 3000);
	m_speeds[FASTEST] = c->Get<double> (m_name, "fastestSpeed", 4000);

	m_pid.setParameters(p, i, d, ff);
}

void Launcher::Disable()
{
	m_roller->SetDutyCycle(0.0);
	m_action->launcher->atSpeed = true;
}

void Launcher::Output()
{
	m_speed = (m_enc->GetStopped()) ? 0.0 : (60.0 / 2.0 / m_enc->GetPeriod());
	m_speed = Util::Clamp<double>(m_speed, 0, m_max_speed * 1.3);
	double targetSpeed;
	switch (m_action->launcher->desiredSpeed)
	{
	case ACTION::LAUNCHER::SLOW:
		targetSpeed = m_speeds[SLOW];
		break;
	case ACTION::LAUNCHER::MEDIUM:
		targetSpeed = m_speeds[MEDIUM];
		break;
	case ACTION::LAUNCHER::FASTEST:
		targetSpeed = m_speeds[FASTEST];
		break;
	}

	switch (m_action->launcher->state)
	{
	case ACTION::LAUNCHER::RUNNING:
		m_pid.setSetpoint(Util::Clamp<double>(targetSpeed, 0, m_max_speed));

		m_pid.setInput(m_speed);
		m_pid.update(1.0 / RobotConfig::LOOP_RATE); // 50 Hz
		m_output = m_pid.getOutput() / m_max_speed;

		static int atSpeedCounter = 0;
		if (fabs(m_pid.getError()) < m_speed_threshold)
		{
			atSpeedCounter++;
		}
		else
		{
			AsyncPrinter::Printf("not at speed\n");
			atSpeedCounter = 0;
		}

		if (atSpeedCounter > m_atSpeedCycles)
		{
			m_action->launcher->atSpeed = true; // w/i 10 rpm
		}
		else
		{
			m_action->launcher->atSpeed = false; // w/i 10 rpm
		}

		break;
	case ACTION::LAUNCHER::DISABLED:
		m_action->launcher->atSpeed = false;
		m_output = 0.0;
		m_pid.reset();
		break;
	}

	if (m_action->launcher->topTrajectory)
	{
		Pneumatics::getInstance()->setTrajectory(true);
	}
	else
	{
		Pneumatics::getInstance()->setTrajectory(false);
	}

	double max_output = m_speed / m_max_speed + m_duty_cycle_delta;
	m_output = min(max_output, m_output);
	m_output = Util::Clamp<double>(m_output, 0, 1.0);

	if (m_action->motorsEnabled)
	{
		m_roller->SetDutyCycle(m_output);
	}
	else
	{
		m_roller->SetDutyCycle(0.0);
	}
	m_roller->ConfigNeutralMode(AsyncCANJaguar::kNeutralMode_Coast);

}

std::string Launcher::GetName()
{
	return m_name;
}

void Launcher::log()
{
	SmartDashboard::GetInstance()->PutString(
			"Launcher State",
			(m_action->launcher->state == ACTION::LAUNCHER::RUNNING) ? "Running"
					: "Disabled");
	SmartDashboard::GetInstance()->PutDouble("Launcher Speed", m_speed);
	SmartDashboard::GetInstance()->PutDouble("Launcher Output", m_output);
	SmartDashboard::GetInstance()->PutDouble("Launcher Max speed", m_max_speed);

	SmartDashboard::GetInstance()->PutDouble("Launcher Setpoint",
			m_pid.getSetpoint());
	SmartDashboard::GetInstance()->PutDouble("Launcher Error", m_pid.getError());
	SmartDashboard::GetInstance()->PutDouble("Launcher P",
			m_pid.getProportionalGain());
	SmartDashboard::GetInstance()->PutDouble("Launcher I",
			m_pid.getIntegralGain());
	SmartDashboard::GetInstance()->PutDouble("Launcher D",
			m_pid.getDerivativeGain());

	SmartDashboard::GetInstance()->PutString("Launcher Trajectory",
			(m_action->launcher->topTrajectory) ? "High" : "Low");

	SmartDashboard::GetInstance()->PutBoolean("Launcher at speed",
			(m_action->launcher->atSpeed));

}
