#include "BallCollector.h"
#include "../ActionData/BallFeedAction.h"
#include "../Config/RobotConfig.h"

BallCollector::BallCollector() :
	m_name("Ball Collector"), m_configsection("bc")
{
	m_roller = new AsyncCANJaguar(RobotConfig::CAN::COLLECTOR, "Collector");
	m_arm = new Solenoid(RobotConfig::DIGITAL_IO::ARM_CYLINDER);
}

BallCollector::~BallCollector()
{
	delete m_roller;
	delete m_arm;
}

void BallCollector::Output()
{
	switch (action->ballfeed->collector_state)
	{
	case ACTION::BALLFEED::COLLECTING:
		m_roller->SetDutyCycle(m_fwd_duty);
		break;
	case ACTION::BALLFEED::REJECTING:
		m_roller->SetDutyCycle(m_rev_duty);
		break;
	case ACTION::BALLFEED::NEUTRAL:
	default:
		m_roller->SetDutyCycle(0.0);
		break;
	}
	m_arm->Set(action->ballfeed->sweepArmOut);
}

void BallCollector::Configure()
{
	m_fwd_duty = Config::GetInstance()->Get<double> (m_configsection,
			"fwdSpeed", 0.3);
	m_rev_duty = Config::GetInstance()->Get<double> (m_configsection,
			"revSpeed", -0.3);
}

std::string BallCollector::GetName()
{
	return m_name;
}

void BallCollector::log()
{
	SmartDashboard * sdb = SmartDashboard::GetInstance();
	std::string s;
	switch (action->ballfeed->collector_state)
	{
	case ACTION::BALLFEED::COLLECTING:
		s = "Collecting";
		break;
	case ACTION::BALLFEED::REJECTING:
		s = "Rejecting";
		break;
	case ACTION::BALLFEED::NEUTRAL:
		s = "Neutral";
		break;
	}
	sdb->PutString("Ball Collector Status", s);
}
