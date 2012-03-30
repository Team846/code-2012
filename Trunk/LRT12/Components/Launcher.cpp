#include "Launcher.h"
#include "Pneumatic/Pneumatics.h"

Launcher::Launcher() :
	Component(), Configurable(), Loggable(), m_name("Launcher")
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

	m_p_conf = ACTION::LAUNCHER::KEY_SHOT_HIGH;
	m_is_changing_speed = true;

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
	m_action->launcher->speed = c->Get<double> (m_name,
			"m_action->launcher->speed", 3000);
	m_atSpeedCycles = c->Get<int> (m_name, "cyclesAtSpeed", 1);

	m_speeds[ACTION::LAUNCHER::FAR_FENDER_SHOT_HIGH] = c->Get<double> (m_name,
			"farFenderHigh", 2000);
	m_speeds[ACTION::LAUNCHER::FAR_FENDER_SHOT_LOW] = c->Get<double> (m_name,
			"farFenderLow", 3000);
	m_speeds[ACTION::LAUNCHER::FENDER_SHOT_HIGH] = c->Get<double> (m_name,
			"fenderHigh", 3000);
	m_speeds[ACTION::LAUNCHER::FENDER_SHOT_LOW] = c->Get<double> (m_name,
			"fenderLow", 3000);
	m_speeds[ACTION::LAUNCHER::KEY_SHOT_HIGH] = c->Get<double> (m_name,
			"keyHigh", 4000);
	m_speeds[ACTION::LAUNCHER::KEY_SHOT_LOW] = c->Get<double> (m_name,
			"keyLow", 3000);

	m_trajectories[ACTION::LAUNCHER::FAR_FENDER_SHOT_HIGH] = true;
	m_trajectories[ACTION::LAUNCHER::FAR_FENDER_SHOT_LOW] = true;
	m_trajectories[ACTION::LAUNCHER::FENDER_SHOT_HIGH] = true;
	m_trajectories[ACTION::LAUNCHER::FENDER_SHOT_LOW] = false;
	m_trajectories[ACTION::LAUNCHER::KEY_SHOT_HIGH] = false;
	m_trajectories[ACTION::LAUNCHER::KEY_SHOT_LOW] = false;

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

	m_action->launcher->speed = m_speeds[m_action->launcher->desiredTarget];

	if (m_p_conf != m_action->launcher->desiredTarget)
	{
		m_is_changing_speed = true;
	}

	switch (m_action->launcher->state)
	{
	case ACTION::LAUNCHER::RUNNING:
		m_pid.setSetpoint(
				Util::Clamp<double>(m_action->launcher->speed, 0, m_max_speed));

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
			//			AsyncPrinter::Printf("not at speed\n");
			atSpeedCounter = 0;
		}

		if (atSpeedCounter > m_atSpeedCycles)
		{
			m_is_changing_speed = false;
			m_action->launcher->atSpeed = true; // w/i 10 rpm
		}
		else
		{
			m_action->launcher->atSpeed = false; // w/i 10 rpm
		}

		static bool wasAtSpeed = false;
		// catch falling edge
		if (wasAtSpeed && !m_action->launcher->atSpeed && !m_is_changing_speed
				&& m_pid.getError() > 0)
		{
			m_action->launcher->ballLaunchCounter++;
		}
		wasAtSpeed = m_action->launcher->atSpeed;

		break;
	case ACTION::LAUNCHER::DISABLED:
		m_action->launcher->atSpeed = false;
		m_output = 0.0;
		m_is_changing_speed = true;
		m_pid.reset();
		break;
	}

	if (m_trajectories[m_action->launcher->desiredTarget])
	{
		Pneumatics::getInstance()->setTrajectory(true, true);
	}
	else
	{
		Pneumatics::getInstance()->setTrajectory(false, true);
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

#define ENABLE_PID_LOGGING 0

void Launcher::log()
{
#if LOGGING_ENABLED
	SmartDashboard * sdb = SmartDashboard::GetInstance();
	sdb->PutString(
			"Launcher State",
			(m_action->launcher->state == ACTION::LAUNCHER::RUNNING) ? "Running"
					: "Disabled");

	sdb->PutString(
			"Launcher Target",
			ACTION::LAUNCHER::launcherConfigStr[m_action->launcher->desiredTarget]);

	sdb->PutDouble("Launcher Speed", m_speed);
	sdb->PutDouble("Launcher Output", m_output);
	sdb->PutDouble("Launcher Setpoint", m_pid.getSetpoint());
#if ENABLE_PID_LOGGING
	sdb->PutDouble("Launcher Max speed", m_max_speed);
	sdb->PutDouble("Launcher Error", m_pid.getError());
	sdb->PutDouble("Launcher P", m_pid.getProportionalGain());
	sdb->PutDouble("Launcher I", m_pid.getIntegralGain());
	sdb->PutDouble("Launcher D", m_pid.getDerivativeGain());
#endif // ENABLE_PID_LOGGING
	sdb->PutString("Launcher Trajectory",
			(m_action->launcher->isFenderShot) ? "High" : "Low");

	sdb->PutBoolean("Launcher at speed", (m_action->launcher->atSpeed));
#endif
}
