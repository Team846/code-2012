#include "Pneumatics.h"

Pneumatics* Pneumatics::m_instance = NULL;

#define PNEUMATICS_DISABLED 1

#define INIT_PULSED_SOLENOID(x, y) (x).solenoid = (y);\
	(x).counter = (m_pulse_length); \
	(x).state = false; \
	(x).pulsed = true;

Pneumatics::Pneumatics() :
	AsyncProcess("Pneumatics"), Configurable(), Loggable(),
			m_name("Pneumatics")
{
	Configure();

#if PNEUMATICS_DISABLED
#warning pneumatics disabled
	printf("Pneumatics disabled\r\n");
#else
	INIT_PULSED_SOLENOID(m_shared, new DoubleSolenoid(
					RobotConfig::SOLENOID_IO::WEDGE_LATCH,
					RobotConfig::SOLENOID_IO::PRESSURE_PLATE));

	INIT_PULSED_SOLENOID(m_trajectory, new DoubleSolenoid(
					RobotConfig::SOLENOID_IO::SHOOTER_SELECT_A,
					RobotConfig::SOLENOID_IO::SHOOTER_SELECT_B));

	INIT_PULSED_SOLENOID(m_shifter, new DoubleSolenoid(
					RobotConfig::SOLENOID_IO::SHIFTER_A,
					RobotConfig::SOLENOID_IO::SHIFTER_B));
	m_shifter.pulsed = false;

	INIT_PULSED_SOLENOID(m_ballcollector, new DoubleSolenoid(
					RobotConfig::SOLENOID_IO::BALL_COLLECTOR_A,
					RobotConfig::SOLENOID_IO::BALL_COLLECTOR_B));

	m_compressor = new Compressor(
			RobotConfig::DIGITAL_IO::COMPRESSOR_PRESSURE_SENSOR_PIN,
			RobotConfig::RELAY_IO::COMPRESSOR_RELAY);
	m_compressor->Start();
#endif

	disableLog();
}

#undef INIT_PULSED_SOLENOID

Pneumatics * Pneumatics::getInstance()
{
	if (m_instance == NULL)
	{
		m_instance = new Pneumatics();
	}
	return m_instance;
}

void Pneumatics::setShifter(bool on, bool force)
{
	if (on != m_shifter.state || force)
	{
		m_shifter.state = on;
		m_shifter.counter = m_pulse_length;
	}
}

void Pneumatics::setBallCollector(bool on, bool force)
{
	if (on != m_ballcollector.state || force)
	{
		m_ballcollector.state = on;
		m_ballcollector.counter = m_pulse_length;
	}
}

void Pneumatics::setTrajectory(bool on, bool force)
{
	if (on != m_trajectory.state || force)
	{
		m_trajectory.state = on;
		m_trajectory.counter = m_pulse_length;
	}
}

void Pneumatics::setLatch(bool on, bool force)
{
	m_mutex = on;
	on = !on;
	if (on != m_shared.state || force)
	{
		m_shared.state = on;
		m_shared.counter = m_pulse_length;
	}
}

void Pneumatics::setPressurePlate(bool on, bool force)
{
	on = !on;
	if (!m_mutex)
	{
		if (on != m_shared.state || force)
		{
			m_shared.state = on;
			m_shared.counter = m_pulse_length;
		}
	}
}

void Pneumatics::Configure()
{
	Config* c = Config::GetInstance();
	m_pulse_length = c->Get<int> (m_name, "pulseLength", 25);
}

void Pneumatics::log()
{
#if LOGGING_ENABLED
	SmartDashboard * sdb = SmartDashboard::GetInstance();
	sdb->PutString("Shared Solenoid State",
			(m_shared.state) ? "Forward" : "Reverse");
	sdb->PutInt("Shared Solenoid Counter", m_shared.counter);
	sdb->PutBoolean("Shared Solenoid Pulsing Enabled", m_shared.pulsed);

	sdb->PutString("BC Solenoid State",
			(m_ballcollector.state) ? "Forward" : "Reverse");
	sdb->PutInt("BC Solenoid Counter", m_ballcollector.counter);
	sdb->PutBoolean("BC Solenoid Pulsing Enabled", m_ballcollector.pulsed);

	sdb->PutString("Trajectory Solenoid State",
			(m_trajectory.state) ? "Forward" : "Reverse");
	sdb->PutInt("Trajectory Solenoid Counter", m_trajectory.counter);
	sdb->PutBoolean("Trajectory Solenoid Pulsing Enabled", m_trajectory.pulsed);

	sdb->PutString("Shifter Solenoid State",
			(m_shifter.state) ? "Forward" : "Reverse");
	sdb->PutInt("Shifter Solenoid Counter", m_shifter.counter);
	sdb->PutBoolean("Shifter Solenoid Pulsing Enabled", m_shifter.pulsed);
#endif
}

void Pneumatics::pulse(PulsedSolenoid * ptr)
{
#if PNEUMATICS_DISABLED
	static uint32_t ctr = 0;
	if (++ctr > 10000)
	{
		AsyncPrinter::Printf("Pneumatics Disabled\r\n");
		ctr = 0;
	}
	return;
#else
	if (ptr->pulsed)
	{
		if (ptr->counter > 0)
		{
			ptr->counter = ptr->counter - 1;
			if (ptr->state)
			{
				ptr->solenoid->Set(DoubleSolenoid::kForward);
			}
			else
			{
				ptr->solenoid->Set(DoubleSolenoid::kReverse);
			}
		}
		else
		{
			ptr->counter = 0;
			ptr->solenoid->Set(DoubleSolenoid::kOff);
		}
	}
	else
	{
		if (ptr->state)
		{
			ptr->solenoid->Set(DoubleSolenoid::kForward);
		}
		else
		{
			ptr->solenoid->Set(DoubleSolenoid::kReverse);
		}
	}
#endif
}

void Pneumatics::setCompressor(bool on)
{
#if not PNEUMATICS_DISABLED
	if (on)
	{
		m_compressor->Start();
	}
	else
	{
		m_compressor->Stop();
	}
#endif
}

Pneumatics::~Pneumatics()
{
	deinit();
#if not PNEUMATICS_DISABLED
	delete m_shared.solenoid;
	delete m_trajectory.solenoid;
	delete m_shifter.solenoid;
	delete m_ballcollector.solenoid;
	m_compressor->Stop();
	delete m_compressor;
#endif
}

void Pneumatics::work()
{
	pulse(&m_shifter);
	pulse(&m_ballcollector);
	pulse(&m_trajectory);
	pulse(&m_shared);
}
