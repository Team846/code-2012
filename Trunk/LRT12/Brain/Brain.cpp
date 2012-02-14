#include "Brain.h"
#include "../ActionData/DriveTrainAction.h"
#include "../Config/DriverStationConfig.h"
#include "../ActionData/ShifterAction.h"
#include "../ActionData/ConfigAction.h"
#include "Joystick.h"

Brain::Brain()
{
	m_teleop_task = new Task("Teleop", (FUNCPTR) Brain::teleopTaskEntryPoint);
	m_auton_task = new Task("Auton", (FUNCPTR) Brain::autonTaskEntryPoint);
	m_driver_stick = new DebouncedJoystick(1,
			DriverStationConfig::NUM_JOYSTICK_BUTTONS,
			DriverStationConfig::NUM_JOYSTICK_AXES);
	m_operator_stick = new DebouncedJoystick(2,
			DriverStationConfig::NUM_JOYSTICK_BUTTONS,
			DriverStationConfig::NUM_JOYSTICK_AXES);

	m_ds = DriverStation::GetInstance();
	missedPackets = 0;
	isTeleop = false;

	actionData = ActionData::GetInstance();
	actionSemaphore = semBCreate(SEM_Q_PRIORITY, SEM_FULL);

	m_auton_task->Start((uint32_t) this);
	m_teleop_task->Start((int32_t) this);
}

Brain::~Brain()
{
	m_auton_task->Stop();
	m_teleop_task->Stop();
	delete m_auton_task;
	delete m_teleop_task;
	delete m_driver_stick;
	delete m_operator_stick;
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
	while (true)
	{
		process();
	}
}

void Brain::teleopTask()
{
	uint32_t lastPacketNum = 0;
	while (true)
	{
		if (lastPacketNum != 0) //don't run this the first time
		{
			if (m_ds->GetPacketNumber() != lastPacketNum + 1)
			{
				//we missed a packet
				missedPackets += m_ds->GetPacketNumber() - lastPacketNum - 1;
			}
		}

		m_driver_stick->Update();
		m_operator_stick->Update();

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
		takeActionSem();
		actionData->drivetrain->rate.drive_control = true;
		actionData->drivetrain->rate.turn_control = true;
		actionData->drivetrain->position.drive_control = false;
		actionData->drivetrain->position.turn_control = false;
		actionData->drivetrain->rate.desiredDriveRate
				= -m_driver_stick->GetAxis(Joystick::kYAxis);
		actionData->drivetrain->rate.desiredTurnRate
				= -m_driver_stick->GetAxis(Joystick::kZAxis);
		if (m_driver_stick->IsButtonJustPressed(2))
		{
			actionData->config->save = true;
		}
		if (m_driver_stick->IsButtonJustPressed(3))
		{
			actionData->config->load = true;
		}
		if (m_driver_stick->IsButtonJustPressed(4))
		{
			actionData->config->apply = true;
		}
		giveActionSem();
	}
}

