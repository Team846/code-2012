#include "BallCollector.h"
#include "../ActionData/BallFeedAction.h"
#include "../Config/RobotConfig.h"

BallCollector::BallCollector() :
	m_name("Ball Collector"), m_configsection("bc")
{
	m_roller = new AsyncCANJaguar(RobotConfig::CAN::COLLECTOR, "Collector");
	m_arm = new DoubleSolenoid(RobotConfig::SOLENOID_IO::BALL_COLLECTOR_A,
			RobotConfig::SOLENOID_IO::BALL_COLLECTOR_B);
}

BallCollector::~BallCollector()
{
	delete m_roller;
	delete m_arm;
}

void BallCollector::Output()
{
	if (action->ballfeed->sweepArmOut)
	{
		m_arm->Set(DoubleSolenoid::kForward);
		m_roller->SetDutyCycle(m_fwd_duty);
	}
	else
	{
		m_arm->Set(DoubleSolenoid::kReverse);
		m_roller->SetDutyCycle(0.0);
	}
}

void BallCollector::Configure()
{
	AsyncPrinter::Printf("Configured");
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
	if (action->ballfeed->sweepArmOut)
	{
		s = "Down/Collecting";
	}
	else
	{
		s = "Up/Idle";
	}
	sdb->PutString("Ball Collector Status", s);
}
