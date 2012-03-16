#include "Pneumatics.h"

Pneumatics* Pneumatics::m_instance = NULL;

#define PNEUMATICS_DISABLED 0

#define INIT_PULSED_SOLENOID(x, y) (x).solenoid = (y);\
	(x).counter = (m_pulse_length); \
	(x).state = false; \
	(x).pulsed = true;

Pneumatics::Pneumatics() :
	Configurable(), Loggable(), m_name("Pneumatics")
{
	Configure();

#if PNEUMATICS_DISABLED
#warning pneumatics disabled
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
#endif

	m_mutex = false;

	disableLog();

	m_task = new Task("PneumaticsTask", (FUNCPTR) Pneumatics::taskEntryPoint,
			Task::kDefaultPriority - 2);
	m_task_sem = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);

	m_is_running = false;
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

void Pneumatics::startBackgroundTask()
{
	m_is_running = true;
	m_task->Start();
}

void Pneumatics::stopBackgroundTask()
{
	if (m_is_running)
	{
		m_is_running = false;
		UINT32 task_id = m_task->GetID();
		m_task->Stop();
		printf("Task 0x%x killed for Pneumatics\n", task_id);
	}
}

void Pneumatics::setShifter(bool on)
{
	if (on != m_shifter.state)
	{
		m_shifter.state = on;
		m_shifter.counter = m_pulse_length;
	}
}

void Pneumatics::setBallCollector(bool on)
{
	if (on != m_ballcollector.state)
	{
		m_ballcollector.state = on;
		m_ballcollector.counter = m_pulse_length;
	}
}

void Pneumatics::setTrajectory(bool on)
{
	if (on != m_trajectory.state)
	{
		m_trajectory.state = on;
		m_trajectory.counter = m_pulse_length;
	}
}

void Pneumatics::setLatch(bool on)
{
	m_mutex = on;
	on = !on;
	if (on != m_shared.state)
	{
		m_shared.state = on;
		m_shared.counter = m_pulse_length;
	}
}

void Pneumatics::setPressurePlate(bool on)
{
	on = !on;
	if (!m_mutex)
	{
		if (on != m_shared.state)
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
}

void Pneumatics::releaseSemaphore()
{
	if (m_task_sem)
		semGive(m_task_sem);
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

Pneumatics::~Pneumatics()
{
	stopBackgroundTask();
	delete m_task;

	int error = semDelete(m_task_sem);
	if (error)
	{
		printf("SemDelete Error=%d\n", error);
	}

#if not PNEUMATICS_DISABLED
	delete m_shared.solenoid;
	delete m_trajectory.solenoid;
	delete m_shifter.solenoid;
	delete m_ballcollector.solenoid;
#endif
}

void Pneumatics::taskEntryPoint()
{
	Pneumatics * p = Pneumatics::getInstance();
	p->task();
}

void Pneumatics::task()
{
	while (m_is_running)
	{
		semTake(m_task_sem, WAIT_FOREVER);
		if (!m_is_running)
		{
			break;
		}
		pulse(&m_shifter);
		pulse(&m_ballcollector);
		pulse(&m_trajectory);
		pulse(&m_shared);
	}
}
