#include "PneumaticShifter.h"
#include "../Util/AsyncPrinter.h"
#include "LRTServo.h"
#include "..\Sensors\DriveEncoders.h"
#include "..\Config\RobotConfig.h"
#include "..\Config\Config.h"
#include "../ActionData/DriveTrainAction.h"
#include "../ActionData/ShifterAction.h"

PneumaticShifter::PneumaticShifter() :
	Component(), encoders(DriveEncoders::GetInstance()),
			config_section("PneumaticShifter")
{
	m_left_sol = new Solenoid(RobotConfig::DIGITAL_IO::SHIFTER_LEFT_SOLENOID);
	m_right_sol = new Solenoid(RobotConfig::DIGITAL_IO::SHIFTER_RIGHT_SOLENOID);
	puts("Constructed PneumaticShifter");
}

PneumaticShifter::~PneumaticShifter()
{
	delete m_left_sol;
	delete m_right_sol;
}

void PneumaticShifter::Configure()
{
}

void PneumaticShifter::Output()
{
	switch (action->shifter->gear)
	{
	case ACTION::GEARBOX::LOW_GEAR:
		m_left_sol->Set(false);
		m_right_sol->Set(false);
		encoders.setHighGear(false);
		break;

	case ACTION::GEARBOX::HIGH_GEAR:
		m_left_sol->Set(true);
		m_right_sol->Set(true);
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
			(action->shifter->gear == ACTION::GEARBOX::HIGH_GEAR) ? "High Gear"
					: "Low Gear");
}
