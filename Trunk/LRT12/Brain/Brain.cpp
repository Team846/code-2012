#include "Brain.h"
#include "../Util/AsyncPrinter.h"
#include "../ActionData/AutonomousAction.h"
#include "Joystick.h"

Brain::Brain()
{
	m_teleop_task = new Task("Teleop", (FUNCPTR) Brain::taskEntryPoint,
			Task::kDefaultPriority - 2);

	m_inputs = new InputParser();
	m_ds = DriverStation::GetInstance();
	missedPackets = 0;
	isTeleop = false;

	action = ActionData::GetInstance();
	actionSemaphore = semBCreate(SEM_Q_PRIORITY, SEM_FULL);

}

void Brain::Start()
{
	m_teleop_task->Start((uint32_t) this);
}

Brain::~Brain()
{
	m_teleop_task->Stop();
	delete m_teleop_task;
	delete m_inputs;
}

void Brain::startTeleop()
{
	isTeleop = true;
}

void Brain::teleopTask()
{
	//	uint32_t lastPacketNum = 0;
	while (true)
	{
		//		if (lastPacketNum != 0) //don't run this the first time
		//		{
		//			if (m_ds->GetPacketNumber() != lastPacketNum + 1)
		//			{
		//				//we missed a packet
		//				missedPackets += m_ds->GetPacketNumber() - lastPacketNum - 1;
		//			}
		//		}

		if (m_ds->IsNewControlData())
		{
			process();
		}
		Wait(0.02);

		//		lastPacketNum = m_ds->GetPacketNumber();
		//		m_ds->WaitForData();
	}
}

void Brain::takeActionSem()
{
	semTake(actionSemaphore, WAIT_FOREVER);
}

void Brain::giveActionSem()
{
	semGive(actionSemaphore);
}

int Brain::taskEntryPoint(uint32_t brain)
{
	Brain *brain_prt = (Brain*) brain;
	brain_prt->teleopTask();
	return 0;
}

void Brain::process()
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

