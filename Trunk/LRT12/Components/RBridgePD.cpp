#include <math.h>
#include "RBridgePD.h"
#include "..\Config\Robotconfig.h"
#include "..\Jaguar\ASyncCANJaguar.h"
#include "..\Config\config.h"
#include "..\ActionData\BPDAction.h"

RBridgePD::RBridgePD() :
	Component(), config(Config::GetInstance()), configSection("Arm")
{
	m_prev_state = ACTION::BPD::IDLE;
	m_cycle_count = 0;
	m_preset_mode = true;
	m_pulse_count = 0;
	m_arm_esc = new AsyncCANJaguar(RobotConfig::CAN::BPD, "Arm");

	m_arm_pot = new AnalogChannel(RobotConfig::ANALOG::POT_ARM);
	// brake when set to 0 to keep the arm in place
	m_arm_esc->ConfigNeutralMode(AsyncCANJaguar::kNeutralMode_Brake);
	Configure();
	printf("Constructed Arm\n");
}

RBridgePD::~RBridgePD()
{
	delete m_arm_esc;
	delete m_arm_pot;
}

void RBridgePD::Configure()
{
	minPosition = config->Get<double> (configSection, "minPosition", 280);
	maxPosition = config->Get<double> (configSection, "maxPosition", 530);

	midPositionDeadband = config->Get<double> (configSection,
			"midPositionDeadband", 10);

	maxPowerUp = config->Get<double> (configSection, "maxPowerUp", 0.30);
	powerRetainUp = config->Get<double> (configSection, "powerRetainUp", 0.10);
	powerDown = config->Get<double> (configSection, "powerDown", -0.15);

	midPowerUp = config->Get<double> (configSection, "midPowerUp", 0.2);
	midPowerDown = config->Get<double> (configSection, "midPowerDown", -0.15);

	pGainDown = config->Get<double> (configSection, "pGainDown", 0.0015);
	pGainUp = config->Get<double> (configSection, "pGainUp", 0.003);
	pGainMid = config->Get<double> (configSection, "pGainMid", 0.01);

	timeoutCycles = (int) (config->Get<int> (configSection, "timeoutMs", 1500)
			* 1.0 / 1000.0 * 50.0 / 1.0);
}

void RBridgePD::Output()
{

	double potValue = m_arm_pot->GetAverageValue();

#ifdef USE_DASHBOARD
	SmartDashboard::Log(potValue, "Arm Pot Value");
#endif

	if (action->master.abort)
	{
		m_arm_esc->SetDutyCycle(0.0);
		action->bridgePD->state = ACTION::BPD::IDLE;
		action->bridgePD->completion_status = ACTION::ABORTED;
		return; // do not allow normal processing (ABORT is not printed...should fix this -dg)
	}

	const bool state_change = (m_prev_state != action->bridgePD->state);
	if (state_change)
		AsyncPrinter::Printf("Arm: %s\n",
				ACTION::BPD::state_string[action->bridgePD->state]);

	if (state_change)
		m_cycle_count = timeoutCycles; // reset timeout

	switch (action->bridgePD->state)
	{
	case ACTION::BPD::PRESET_TOP:
		action->bridgePD->completion_status = ACTION::IN_PROGRESS;

		// don't merely switch to the IDLE state, as the caller will likely
		// set the state each time through the loop
		if (--m_cycle_count < 0)
		{
			action->bridgePD->completion_status = ACTION::FAILURE;
			m_arm_esc->SetDutyCycle(0.0);
			break; // timeout overrides everything
		}

		if (potValue >= maxPosition)
		{
			action->bridgePD->completion_status = ACTION::SUCCESS;
			m_arm_esc->SetDutyCycle(powerRetainUp);
			// cycleCount will never get decremented below 0, so powerRetainUp
			// will be maintained
			m_cycle_count = 100;
		}
		else //we have not yet hit the setpoint
		{
			action->bridgePD->completion_status = ACTION::IN_PROGRESS;
			double error = maxPosition - potValue;

			m_arm_esc->SetDutyCycle(
					Util::Max<double>(
							Util::Min<double>(maxPowerUp * 1.5, error * pGainUp),
							0.15));

		}
		break;

	case ACTION::BPD::PRESET_BOTTOM:
		action->bridgePD->completion_status = ACTION::IN_PROGRESS;

		if (--m_cycle_count < 0)
		{
			action->bridgePD->completion_status = ACTION::FAILURE;
			m_arm_esc->SetDutyCycle(0.0);
			break; // timeout overrides everything
		}

		if (potValue <= minPosition)
		{
			action->bridgePD->completion_status = ACTION::SUCCESS;
			m_arm_esc->SetDutyCycle(0.0); // don't go below the min position
			// cycleCount will never get decremented below 0, so powerRetainUp
			// will be maintained
			m_cycle_count = 100;
		}
		else
		{
			action->bridgePD->completion_status = ACTION::IN_PROGRESS;
			double error = minPosition - potValue;
			m_arm_esc->SetDutyCycle(
					Util::Max<double>(powerDown, error * pGainDown));
			//            AsynchronousPrinter::Printf("setpoint %.3f\n",Util::Max<double>(powerDown, error*0.0015));
		}
		break;
	case ACTION::BPD::IDLE:
		action->bridgePD->completion_status = ACTION::SUCCESS;
		m_arm_esc->SetDutyCycle(0.0);
		break;
	default:
		AsyncPrinter::Printf("Arm: ERROR Unknown State\n");

	}

	m_prev_state = action->bridgePD->state;

	//Print diagnostics
	static ACTION::eCompletionStatus lastDoneState = ACTION::UNSET;
	if (lastDoneState != action->bridgePD->completion_status)
		AsyncPrinter::Printf("Arm: Status=%s\n",
				ACTION::status_string[action->bridgePD->completion_status]);
	lastDoneState = action->bridgePD->completion_status;
}

string RBridgePD::GetName()
{
	return "Arm";
}
