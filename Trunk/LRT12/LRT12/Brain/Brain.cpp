#include "Brain.h"
#include "../Util/AsyncPrinter.h"
#include "../ActionData/AutonomousAction.h"
#include "Joystick.h"

Brain::Brain() :
	SyncProcess("BrainTask")
{
	m_inputs = new InputParser();
	m_ds = DriverStation::GetInstance();
	missedPackets = 0;

	action = ActionData::GetInstance();
}

Brain::~Brain()
{
	deinit();
	delete m_inputs;
}

void Brain::work()
{
	if (m_ds->IsNewControlData())
	{
		static bool runonce = true;
		//This heuristic may eventually have to change if brain is to do processing during disabled
		if (m_ds->IsAutonomous())
		{
			AutonomousAction * a = ActionData::GetInstance()->auton;
			if (m_ds->IsEnabled())
			{
				if (runonce)
				{
					runonce = false;
					a->state = ACTION::AUTONOMOUS::AUTON_MODE;
				}
				if (a->state != ACTION::AUTONOMOUS::TELEOP)
				{
					a->state = ACTION::AUTONOMOUS::AUTON_MODE;
				}
			}
			else
			{
				a->state = ACTION::AUTONOMOUS::TELEOP;
				runonce = true;
			}
		}
		else if (m_ds->IsOperatorControl())
		{
			runonce = true;
			m_inputs->ProcessInputs();
		}
	}
}

