#include "SharedCylinder.h"
#include "../../Config/RobotConfig.h"

SharedSolenoid* SharedSolenoid::instance = NULL;

SharedSolenoid* SharedSolenoid::GetInstance()
{
	if (instance == NULL)
		instance = new SharedSolenoid();
	return instance;
}

SharedSolenoid::SharedSolenoid()
{
	m_solenoid = new DoubleSolenoid(RobotConfig::SOLENOID_IO::PRESSURE_PLATE,
			RobotConfig::SOLENOID_IO::WEDGE_LATCH);
	pressurePlateEnabled = false;
	latchEnabled = false;
}

SharedSolenoid::~SharedSolenoid()
{
}

void SharedSolenoid::EnablePressurePlate()
{
	//	if (!latchEnabled)
	//	{
	m_solenoid->Set(DoubleSolenoid::kReverse);
	pressurePlateEnabled = true;
	//	}
}

void SharedSolenoid::EnableLatch()
{
	m_solenoid->Set(DoubleSolenoid::kForward);
	latchEnabled = true;
}

void SharedSolenoid::DisableLatch()
{
	EnablePressurePlate();
	latchEnabled = false;
	//	m_solenoid->Set(DoubleSolenoid::kForward);
	//	if (!pressurePlateEnabled)
	//		m_solenoid->Set(DoubleSolenoid::kOff);
	//	else
	//		m_solenoid->Set(DoubleSolenoid::kForward);
	//	latchEnabled = false;
}

void SharedSolenoid::DisablePressurePlate()
{
	EnableLatch();
	pressurePlateEnabled = false;
	//	if (!latchEnabled)
	//		m_solenoid->Set(DoubleSolenoid::kOff);
	//	else
	//		m_solenoid->Set(DoubleSolenoid::kReverse);
	//	pressurePlateEnabled = false;
}

void SharedSolenoid::taskEntry(uint32_t thingy)
{
	SharedSolenoid *ptr = (SharedSolenoid *) thingy;
	ptr->task();
}

void SharedSolenoid::task()
{

}
