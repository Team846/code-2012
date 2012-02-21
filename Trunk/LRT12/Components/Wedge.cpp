#include "Wedge.h"

Wedge::Wedge() :
	Component(), m_name("Wedge")
{
	m_spike = new Relay(RobotConfig::RELAY_IO::WEDGE_SPIKE,
			Relay::kBothDirections);
	m_bottomlimit = new DigitalInput(
			RobotConfig::DIGITAL_IO::WEDGE_LIMIT_BOTTOM);
	m_toplimit = new DigitalInput(RobotConfig::DIGITAL_IO::WEDGE_LIMIT_TOP);
	m_latch = new Solenoid(RobotConfig::SOLENOID_IO::WEDGE_LATCH);

	Configure();

	m_ctr = 0;

	AsyncPrinter::Printf("Constructed Wedge\n");
}

Wedge::~Wedge()
{
	delete m_latch;
	delete m_bottomlimit;
	delete m_toplimit;
	delete m_spike;
}

void Wedge::Configure()
{
	Config * config = Config::GetInstance();
	m_pulse_down = config->Get<int> (m_name, "pulseDown", 25);
	m_pulse_up = config->Get<int> (m_name, "pulseUp", 25);
}

void Wedge::Disable()
{
	m_latch->Set(false);
	m_spike->Set(Relay::kOff);
}

void Wedge::Output()
{
	if (m_lastState != m_action->wedge->state)
	{
		if (m_action->wedge->state == ACTION::WEDGE::PRESET_TOP
				|| m_action->wedge->state == ACTION::WEDGE::PRESET_BOTTOM)
		{
			m_ctr = 0;
		}
	}

	switch (m_action->wedge->state)
	{
	case ACTION::WEDGE::PRESET_TOP:
		m_action->wedge->completion_status = ACTION::IN_PROGRESS;

		m_latch->Set(true);

		if (m_toplimit->Get() || ++m_ctr >= m_pulse_up)
		{
			if (m_ctr >= m_pulse_up && !m_toplimit->Get())
			{
				m_action->wedge->completion_status = ACTION::FAILURE;
			}
			else
			{
				m_action->wedge->completion_status = ACTION::SUCCESS;
			}
			m_spike->Set(Relay::kOff);
		}
		else
		{
			m_spike->Set(Relay::kForward);
		}
		break;
	case ACTION::WEDGE::PRESET_BOTTOM:
		m_action->wedge->completion_status = ACTION::IN_PROGRESS;

		m_latch->Set(false);

		if (m_bottomlimit->Get() || ++m_ctr >= m_pulse_down)
		{
			if (m_ctr >= m_pulse_down && !m_bottomlimit->Get())
			{
				m_action->wedge->completion_status = ACTION::FAILURE;
			}
			else
			{
				m_action->wedge->completion_status = ACTION::SUCCESS;
			}

			m_spike->Set(Relay::kOff);
		}
		else
		{
			m_action->wedge->completion_status = ACTION::IN_PROGRESS;

			m_spike->Set(Relay::kReverse);
		}
		break;
	case ACTION::WEDGE::IDLE:
		m_latch->Set(false);
		m_spike->Set(Relay::kOff);
		break;
	}

	m_lastState = m_action->wedge->state;
}

string Wedge::GetName()
{
	return m_name;
}

void Wedge::log()
{
	SmartDashboard * sdb = SmartDashboard::GetInstance();

	std::string pos;
	switch (m_action->wedge->state)
	{
	case ACTION::WEDGE::IDLE:
		pos = "Idle";
		break;
	case ACTION::WEDGE::PRESET_BOTTOM:
		pos = "Down";
		break;
	case ACTION::WEDGE::PRESET_TOP:
		pos = "Up";
		break;
	}
	sdb->PutString("Wedge Position Status", pos);

	std::string stat;
	switch (m_action->wedge->completion_status)
	{
	case ACTION::IN_PROGRESS:
		stat = "In Progress";
		break;
	case ACTION::ABORTED:
		stat = "Aborted";
		break;
	case ACTION::FAILURE:
		stat = "Failed";
		break;
	case ACTION::SUCCESS:
		stat = "Success";
		break;
	case ACTION::UNSET:
		stat = "Unset";
		break;
	}
	sdb->PutString("Wedge Completion Status", stat);
}
