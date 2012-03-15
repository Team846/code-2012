#include "PneumaticShifter.h"
#include "../Util/AsyncPrinter.h"
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
	puts("Constructed PneumaticShifter");
}

PneumaticShifter::~PneumaticShifter()
{
}

void PneumaticShifter::Configure()
{
}

void PneumaticShifter::Disable()
{
}

void PneumaticShifter::Output()
{
	switch (m_action->shifter->gear)
	{
	case ACTION::GEARBOX::LOW_GEAR:
		Pneumatics::getInstance()->setShifter(false, true);
		encoders.setHighGear(false);
		break;

	case ACTION::GEARBOX::HIGH_GEAR:
		Pneumatics::getInstance()->setShifter(true, true);
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
#if LOGGING_ENABLED
	SmartDashboard::GetInstance()->PutString(
			"Robot Gear",
			(m_action->shifter->gear == ACTION::GEARBOX::HIGH_GEAR) ? "High Gear"
			: "Low Gear");
#endif
}
