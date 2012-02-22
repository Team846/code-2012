#include "BallFeeder.h"
#include "../Config/RobotConfig.h"
#include "../Config/Config.h"
#include "../ActionData/BallFeedAction.h"
#include "../ActionData/BPDAction.h"
#include "../ActionData/LauncherAction.h"
#include "Pneumatic/Pneumatics.h"

BallFeeder::BallFeeder() :
	m_name("Ball Feeder"), m_configsection("bf")
{
	m_roller[FRONT] = new AsyncCANJaguar(RobotConfig::CAN::FEEDER_FRONT,
			"BF_FWD");
	m_roller[BACK]
			= new AsyncCANJaguar(RobotConfig::CAN::FEEDER_BACK, "BF_REV");
	m_roller[INTAKE] = new AsyncCANJaguar(RobotConfig::CAN::FRONT_INTAKE,
			"INTAKE");

	loadTimer = 0;
}

BallFeeder::~BallFeeder()
{
	delete m_roller[FRONT];
	delete m_roller[BACK];
	delete m_roller[INTAKE];
}

void BallFeeder::Disable()
{
	double front, back, intake;
	front = (0.0);
	back = (0.0);
	intake = (0.0);

	m_roller[FRONT]->SetDutyCycle(front);
	m_roller[BACK]->SetDutyCycle(back);
	m_roller[INTAKE]->SetDutyCycle(intake);
	Pneumatics::getInstance()->setPressurePlate(false);
	//	SharedSolenoid::GetInstance()->DisablePressurePlate();
}

void BallFeeder::Output()
{
	double front, back, intake;
	switch (m_action->ballfeed->feeder_state)
	{
	case ACTION::BALLFEED::FEEDING:
//			AsyncPrinter::Printf("Feeding\n");
		front = (m_fwd_duty[FRONT]);
		back = (m_fwd_duty[BACK]);
		intake = (m_fwd_duty[INTAKE]);
		break;
	case ACTION::BALLFEED::HOLDING:
//			AsyncPrinter::Printf("Holding\n");
		front = (m_holding_duty[FRONT]);
		back = (m_holding_duty[BACK]);
		intake = (0.0);
		break;
	case ACTION::BALLFEED::PURGING:
//			AsyncPrinter::Printf("Purging\n");
		front = (m_rev_duty[FRONT]);
		back = (m_rev_duty[BACK]);
		intake = (m_rev_duty[INTAKE]);
		break;
	}

	if (m_action->ballfeed->attemptToLoadRound)
	{
		if (m_action->launcher->atSpeed)
		{
			Pneumatics::getInstance()->setPressurePlate(true);
			front = (m_fwd_duty[FRONT]);
			back = (m_fwd_duty[BACK]);
		}
		else
		{
			Pneumatics::getInstance()->setPressurePlate(false);
			//			front=(m_holding_duty[FRONT]);
			//			back=(m_holding_duty[BACK]);
			front = (0.0);
			back = (0.0);

		}
	}
	else
	{
		Pneumatics::getInstance()->setPressurePlate(false);
		//		SharedSolenoid::GetInstance()->DisablePressurePlate();
	}
	//	if (loading)
	//	{
	//		static int e = 0;
	//		if (++e > 70)
	//			loading = false;
	////		if (!m_action->launcher->atSpeed)
	////			loading = false;
	//	}
	//	else
	//	{
	//	}
	//	
	//	if (m_action->ballfeed->attemptToLoadRound)
	//	{
	//		loading = true;
	//		m_action->ballfeed->attemptToLoadRound = false;
	//	}
	if (!m_action->motorsEnabled)
	{
		front = (0.0);
		back = (0.0);
		intake = (0.0);
	}
	m_roller[FRONT]->SetDutyCycle(front);
	m_roller[BACK]->SetDutyCycle(back);
	m_roller[INTAKE]->SetDutyCycle(intake);
	
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

	m_holding_duty[FRONT] = c->Get<double> (m_configsection,
			"front_holding_duty", 0.3);
	m_holding_duty[BACK] = c->Get<double> (m_configsection,
			"back_holding_duty", 0.3);
}

void BallFeeder::log()
{
	SmartDashboard * sdb = SmartDashboard::GetInstance();
	sdb->PutBoolean("Round status", m_action->ballfeed->attemptToLoadRound);
	std::string s;
	switch (m_action->ballfeed->feeder_state)
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
