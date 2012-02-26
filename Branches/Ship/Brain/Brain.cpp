#include "Brain.h"
#include "../Util/AsyncPrinter.h"
#include "Joystick.h"

Brain::Brain()
{
	m_teleop_task = new Task("Teleop", (FUNCPTR) Brain::teleopTaskEntryPoint,
			Task::kDefaultPriority - 2);
	m_auton_task = new Task("Auton", (FUNCPTR) Brain::autonTaskEntryPoint,
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
	m_auton_task->Start((uint32_t) this);
	m_teleop_task->Start((uint32_t) this);
}

Brain::~Brain()
{
	m_auton_task->Stop();
	m_teleop_task->Stop();
	delete m_auton_task;
	delete m_teleop_task;
}

void Brain::startAuton()
{
	isTeleop = false;
}

void Brain::startTeleop()
{
	isTeleop = true;
}

void Brain::autonTask()
{
	//TODO change me to linear 
	//auton code
	//	while (true)
	//	{
	//		process();
	//	}
}

void Brain::teleopTask()
{
	uint32_t lastPacketNum = 0;
	while (true)
	{
		//		takeActionSem();
		if (lastPacketNum != 0) //don't run this the first time
		{
			if (m_ds->GetPacketNumber() != lastPacketNum + 1)
			{
				//we missed a packet
				missedPackets += m_ds->GetPacketNumber() - lastPacketNum - 1;
			}
		}

		process();

		lastPacketNum = m_ds->GetPacketNumber();
		//TODO check that this is not a busy wait
		m_ds->WaitForData();
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

int Brain::autonTaskEntryPoint(uint32_t brain)
{
	Brain *brain_prt = (Brain*) brain;
	brain_prt->autonTask();
	return 0;
}

int Brain::teleopTaskEntryPoint(uint32_t brain)
{
	Brain *brain_prt = (Brain*) brain;
	brain_prt->teleopTask();
	return 0;
}

void Brain::process()
{
	//This heuristic may eventually have to change if brain is to do processing during disabled
	if (m_ds->IsAutonomous())
	{

	}
	else if (m_ds->IsOperatorControl())
	{
#warning position drive does not work yet
		m_inputs->ProcessInputs();
		
		//		giveActionSem();
	}
}
