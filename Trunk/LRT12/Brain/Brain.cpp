#include "Brain.h"
#include "../Config/DriverStationConfig.h"

Brain::Brain() :
			m_teleop_task("Teleop", (FUNCPTR) Brain::teleopTaskEntryPoint),
			m_auton_task("Auton", (FUNCPTR) Brain::autonTaskEntryPoint),
			m_driver_stick(1, DriverStationConfig::NUM_JOYSTICK_BUTTONS,
					DriverStationConfig::NUM_JOYSTICK_AXES),
			m_operator_stick(2, DriverStationConfig::NUM_JOYSTICK_BUTTONS,
					DriverStationConfig::NUM_JOYSTICK_AXES)
{
	m_ds = DriverStation::GetInstance();
	missedPackets = 0;
	isTeleop = false;

	actionData = ActionData::GetInstance();

	m_auton_task.Start((uint32_t) this);
	m_teleop_task.Start((int32_t) this);
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

		m_driver_stick.Update();
		m_operator_stick.Update();

		process();

		lastPacketNum = m_ds->GetPacketNumber();
		//TODO check that this is not a busy wait
		m_ds->WaitForData();
	}
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
	//TODO to something
	//This heuristic may eventually have to change
	if (m_ds->IsAutonomous())
	{

	}
	else if (m_ds->IsOperatorControl())
	{
	}
}