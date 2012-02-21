#include "PneumaticShifter.h"
#include "../Util/AsyncPrinter.h"
#include "LRTServo.h"
#include "..\Sensors\DriveEncoders.h"
#include "..\Config\RobotConfig.h"
#include "..\Config\Config.h"
#include "../ActionData/DriveTrainAction.h"
#include "../ActionData/ShifterAction.h"
#include "Pneumatic/Pneumatics.h"
#include "DoubleSolenoid.h"

PneumaticShifter::PneumaticShifter() :
	Component(), encoders(DriveEncoders::GetInstance()),
			config_section("PneumaticShifter")
{
	//	m_solenoid = new DoubleSolenoid(RobotConfig::SOLENOID_IO::SHIFTER_A,
	//			RobotConfig::SOLENOID_IO::SHIFTER_B);
	puts("Constructed PneumaticShifter");
}

PneumaticShifter::~PneumaticShifter()
{
	//	delete m_solenoid;
}

void PneumaticShifter::Configure()
{
}

void PneumaticShifter::Disable()
{
	//	m_solenoid->Set(DoubleSolenoid::kOff);
}

void PneumaticShifter::Output()
{
	switch (m_action->shifter->gear)
	{
	case ACTION::GEARBOX::LOW_GEAR:
		//		AsyncPrinter::Printf("Low\n");
		Pneumatics::getInstance()->setShifter(true);
		//		m_solenoid->Set(DoubleSolenoid::kForward);
		encoders.setHighGear(false);
		break;

	case ACTION::GEARBOX::HIGH_GEAR:
		//		AsyncPrinter::Printf("High\n");
		Pneumatics::getInstance()->setShifter(false);
		//		m_solenoid->Set(DoubleSolenoid::kReverse);
		encoders.setHighGear(true);
		break;

	default:
		AsyncPrinter::Printf("Fatal: %s:%d\n", __FILE__, __LINE__);
		break;
	}
}

string PneumaticShifter::GetName()
{
	return "PneumaticShifter";
}

void PneumaticShifter::log()
{
	SmartDashboard::GetInstance()->PutString(
			"Robot Gear",
			(m_action->shifter->gear == ACTION::GEARBOX::HIGH_GEAR) ? "High Gear"
					: "Low Gear");
}
