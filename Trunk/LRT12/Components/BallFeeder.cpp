#include "BallFeeder.h"
#include "../Config/RobotConfig.h"
#include "../Config/Config.h"
#include "../ActionData/BallFeedAction.h"


BallFeeder::BallFeeder() :
	m_name("Ball Feeder"), m_configsection("bf")
{
	m_roller[FRONT] = new AsyncCANJaguar(RobotConfig::CAN::FEEDER_FRONT,
			"BF_FWD");
	m_roller[BACK]
			= new AsyncCANJaguar(RobotConfig::CAN::FEEDER_BACK, "BF_REV");
	m_roller[INTAKE] = new AsyncCANJaguar(RobotConfig::CAN::FRONT_INTAKE,
			"INTAKE");
	m_pressure_plate = new Relay(RobotConfig::RELAY_IO::PRESSURE_PLATE);
	
	loadTimer = 0;
}

BallFeeder::~BallFeeder()
{
	delete m_roller[FRONT];
	delete m_roller[BACK];
	delete m_roller[INTAKE];
	delete m_pressure_plate;
}

void BallFeeder::Disable()
{
	m_roller[FRONT]->SetDutyCycle(0.0);
	m_roller[BACK]->SetDutyCycle(0.0);
	m_roller[INTAKE]->SetDutyCycle(0.0);
}

void BallFeeder::Output()
{
	switch (action->ballfeed->feeder_state)
	{
	case ACTION::BALLFEED::FEEDING:
		m_roller[FRONT]->SetDutyCycle(m_fwd_duty[FRONT]);
		m_roller[BACK]->SetDutyCycle(m_fwd_duty[BACK]);
		m_roller[INTAKE]->SetDutyCycle(m_fwd_duty[INTAKE]);
		break;
	case ACTION::BALLFEED::HOLDING:
		m_roller[FRONT]->SetDutyCycle(m_holding_duty[FRONT]);
		m_roller[BACK]->SetDutyCycle(m_holding_duty[BACK]);
		m_roller[INTAKE]->SetDutyCycle(0.0);
		break;
	case ACTION::BALLFEED::PURGING:
		m_roller[FRONT]->SetDutyCycle(m_rev_duty[FRONT]);
		m_roller[BACK]->SetDutyCycle(m_rev_duty[BACK]);
		m_roller[INTAKE]->SetDutyCycle(m_rev_duty[INTAKE]);
		break;
	}
	
	if (loading)
	{
		m_pressure_plate->Set(Relay::kForward);
		loadTimer++;
		if (loadTimer >= cyclesToLoad)
			loading = false;
	}
	else
	{
		m_pressure_plate->Set(Relay::kOff);
	}
	
	if (action->ballfeed->attemptToLoadRound)
	{
		loading = true;
		action->ballfeed->attemptToLoadRound = false;
	}
}

void BallFeeder::Configure()
{
	Config * c = Config::GetInstance();
	m_fwd_duty[FRONT] = c->Get<double> (m_configsection, "front_fwd_duty", 0.3);
	m_fwd_duty[BACK] = c->Get<double> (m_configsection, "back_fwd_duty", 0.3);
	m_fwd_duty[INTAKE] = c->Get<double> (m_configsection, "intake_fwd_duty",
			1.0);
	m_rev_duty[FRONT]
			= c->Get<double> (m_configsection, "front_rev_duty", -0.3);
	m_rev_duty[BACK] = c->Get<double> (m_configsection, "back_rev_duty", -0.3);
	m_rev_duty[INTAKE] = c->Get<double> (m_configsection, "intake_fwd_duty",
			-1.0);
	cyclesToLoad = c->Get<int>(m_configsection, "cyclesToLoad", 40);
	
	m_holding_duty[FRONT] = c->Get<double> (m_configsection, "front_holding_duty", 0.3);
	m_holding_duty[BACK] = c->Get<double> (m_configsection, "back_holding_duty", 0.3);
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
