#include "Wedge.h"
#include "Pneumatic/Pneumatics.h"

Wedge::Wedge() :
	Component(), m_name("Wedge")
{
	m_spike = new Relay(RobotConfig::RELAY_IO::WEDGE_SPIKE,
			Relay::kBothDirections);

	Configure();

	m_ctr = 0;

	AsyncPrinter::Printf("Constructed Wedge\n");
	m_lastState = ACTION::WEDGE::IDLE;
}

Wedge::~Wedge()
{
	delete m_spike;
}

void Wedge::Configure()
{
	Config * config = Config::GetInstance();
	m_pulse_down = config->Get<int> (m_name, "pulseDown", 70);
	m_pulse_up = config->Get<int> (m_name, "pulseUp", 45);
	m_dither = config->Get<int> (m_name, "dither", 5);
}

void Wedge::Disable()
{
	m_spike->Set(Relay::kOff);
	Pneumatics::getInstance()->setLatch(true);
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
		Pneumatics::getInstance()->setLatch(false);

		if (++m_ctr >= m_pulse_up)
		{
			m_action->wedge->completion_status = ACTION::SUCCESS;
			m_spike->Set(Relay::kOff);
		}
		else
		{
			if (m_ctr % m_dither <= 1)
			{
				m_spike->Set(Relay::kReverse);
			}
			else
			{
				m_spike->Set(Relay::kOff);
			}
		}
		break;
	case ACTION::WEDGE::PRESET_BOTTOM:
		m_action->wedge->completion_status = ACTION::IN_PROGRESS;

		Pneumatics::getInstance()->setLatch(true);

		if (++m_ctr >= m_pulse_down)
		{
			m_action->wedge->completion_status = ACTION::SUCCESS;

			m_spike->Set(Relay::kOff);
		}
		else
		{
			m_action->wedge->completion_status = ACTION::IN_PROGRESS;

			if (m_ctr % m_dither <= 1)
			{
				m_spike->Set(Relay::kForward);
			}
			else
			{
				m_spike->Set(Relay::kOff);
			}
		}
		break;
	case ACTION::WEDGE::IDLE:
		Pneumatics::getInstance()->setLatch(false);
		m_spike->Set(Relay::kOff);
		break;
	}
	m_lastState = m_action->wedge->state;

	if (!m_action->motorsEnabled)
	{
		m_spike->Set(Relay::kOff);
	}
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
