#include "BallFeeder.h"
#include "../Config/RobotConfig.h"
#include "../Config/Config.h"
#include "../ActionData/BallFeedAction.h"

#define PRESSURE_PLATE_ENABLED 0

BallFeeder::BallFeeder() :
	m_name("Ball Feeder"), m_configsection("bf")
{
	m_roller[FRONT]
			= new AsyncCANJaguar(RobotConfig::CAN::FEEDER_FWD, "BF_FWD");
	m_roller[BACK] = new AsyncCANJaguar(RobotConfig::CAN::FEEDER_REV, "BF_REV");
#if PRESSURE_PLATE_ENABLED
	m_pressure_plate = new Relay(RobotConfig::RELAY_IO::PRESSURE_PLATE);
#endif
}

BallFeeder::~BallFeeder()
{
	delete m_roller[FRONT];
	delete m_roller[BACK];
#if PRESSURE_PLATE_ENABLED
	delete m_pressure_plate;
#endif
}

void BallFeeder::Output()
{
	switch (action->ballfeed->feeder_state)
	{
	case ACTION::BALLFEED::FEEDING:
		m_roller[FRONT]->SetDutyCycle(m_fwd_duty[FRONT]);
		m_roller[BACK]->SetDutyCycle(m_fwd_duty[BACK]);
		break;
	case ACTION::BALLFEED::HOLDING:
		m_roller[FRONT]->SetDutyCycle(0.0);
		m_roller[BACK]->SetDutyCycle(0.0);
		break;
	case ACTION::BALLFEED::PURGING:
		m_roller[FRONT]->SetDutyCycle(m_rev_duty[FRONT]);
		m_roller[BACK]->SetDutyCycle(m_rev_duty[BACK]);
		break;
	}
#if PRESSURE_PLATE_ENABLED
	m_pressure_plate->Set(action->ballfeed->attemptToLoadRound);
#endif
}

void BallFeeder::Configure()
{
	Config * c = Config::GetInstance();
	m_fwd_duty[FRONT] = c->Get<double> (m_configsection, "front_fwd_duty", 0.3);
	m_fwd_duty[BACK] = c->Get<double> (m_configsection, "back_fwd_duty", 0.3);
	m_rev_duty[FRONT]
			= c->Get<double> (m_configsection, "front_rev_duty", -0.3);
	m_rev_duty[BACK] = c->Get<double> (m_configsection, "back_rev_duty", -0.3);
}

void BallFeeder::log()
{
	SmartDashboard * sdb = SmartDashboard::GetInstance();
	sdb->PutBoolean("Round status", action->ballfeed->attemptToLoadRound);
	std::string s;
	switch (action->ballfeed->feeder_state)
	{
	case ACTION::BALLFEED::FEEDING:
		s = "Feeding";
		break;
	case ACTION::BALLFEED::HOLDING:
		s = "Holding";
		break;
	case ACTION::BALLFEED::PURGING:
		s = "Purging";
		break;
	}
	sdb->PutBoolean("Ball Feeder Status", s.c_str());
}

std::string BallFeeder::GetName()
{
	return m_name;
}
