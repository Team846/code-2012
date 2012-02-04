#include "TBridgePD.h"

TBridgePD::TBridgePD()
	: Component()
	, m_name("BPD")
	, m_configSection("BPD")
{
	m_bpdPot = new AnalogChannel(RobotConfig::ANALOG::POT_ARM);
	
	m_bpdEsc = new AsyncCANJaguar(RobotConfig::CAN::BPD, "BPD");
	
	Configure();
	
	AsyncPrinter::Printf("Constructed Bridge PD\n");
}

TBridgePD::~TBridgePD()
{
	delete m_bpdPot;
}

void TBridgePD::Configure()
{
	Config * config = Config::GetInstance();

	/* TODO: default values... */
	m_minPosition 	= config->Get<double>(m_configSection, "minPosition", 0.0f);
	m_maxPosition	= config->Get<double>(m_configSection, "maxPosition", 0.0f);
	
	m_maxPowerUp 	= config->Get<double>(m_configSection, "maxPowerUp", 0.0f);
	m_powerDown		= config->Get<double>(m_configSection, "powerDown", 0.0f);
	
	m_pGainUp		= config->Get<double>(m_configSection, "pGainUp", 0.0f);
	m_pGainDown		= config->Get<double>(m_configSection, "pGainDown", 0.0f);
	
	m_powerRetainUp	= config->Get<double>(m_configSection, "powerRetainUp", 0.0f);
}

void TBridgePD::Output()
{
	double potValue = m_bpdPot->GetAverageValue();

#ifdef USE_DASHBOARD
	SmartDashboard:Log(potValue, "BPD Pot Value")
#endif
	
	bool updateNeeded = m_lastState != action->bridgePD->state;
	
	if(action->master.abort)
	{
		m_bpdEsc->SetDutyCycle(0.0f);
		
		action->bridgePD->completion_status = ACTION::ABORTED;
		action->bridgePD->state 			= ACTION::BPD::IDLE;
		
		return;
	}

	switch(action->bridgePD->state)
	{
		case ACTION::BPD::PRESET_TOP:
			action->bridgePD->completion_status = ACTION::IN_PROGRESS;
			
			if(potValue >= m_maxPosition)
			{
				action->bridgePD->completion_status = ACTION::SUCCESS;
				m_bpdEsc->SetDutyCycle(m_powerRetainUp);
			}
			else
			{
				double error = m_maxPosition - potValue;
				
				m_bpdEsc->SetDutyCycle(Util::Max<double>(Util::Min<double>(m_maxPowerUp*1.5, error*m_pGainUp), 0.15));
			}
			break;
		case ACTION::BPD::PRESET_BOTTOM:
			action->bridgePD->completion_status = ACTION::IN_PROGRESS;
						
			if(potValue <= m_maxPosition)
			{
				action->bridgePD->completion_status = ACTION::SUCCESS;
				
				m_bpdEsc->SetDutyCycle(0.0F);
			}
			else
			{
				action->bridgePD->completion_status = ACTION::IN_PROGRESS;
				
				double error = m_minPosition - potValue;
			
				m_bpdEsc->SetDutyCycle(Util::Max<double>(m_powerDown, error*m_pGainDown));
			}
			break;
	}
	
	m_lastState = action->bridgePD->state;
}

string TBridgePD::GetName()
{
	return m_name;
}
