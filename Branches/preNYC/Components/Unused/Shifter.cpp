#include "Shifter.h"
#include "../../Util/AsyncPrinter.h"
#include "LRTServo.h"
#include "../..\Sensors\DriveEncoders.h"
#include "../..\Config\RobotConfig.h"
#include "../..\Config\Config.h"
#include "../../ActionData/DriveTrainAction.h"
#include "../../ActionData/ShifterAction.h"

Shifter::Shifter() :
	Component(), encoders(DriveEncoders::GetInstance()),
			config_section("Shifter")
{
	leftServo = new LRTServo(RobotConfig::PWM::LEFT_GEARBOX_SERVO,
			"Left Shift Servo");
	rightServo = new LRTServo(RobotConfig::PWM::RIGHT_GEARBOX_SERVO,
			"Right Shift Servo");
	puts("Constructed Shifter");
	highGearServoValLeft = highGearServoValRight = lowGearServoValLeft
			= lowGearServoValRight = 1500;
	servoDisableDelay = servoDisableTimer = 0;
	Configure();
}

Shifter::~Shifter()
{
	delete leftServo;
	delete rightServo;

}

void Shifter::Configure()
{
	// TODO: add servo values into config
	Config * config = Config::GetInstance();
	lowGearServoValLeft = config->Get<int> (config_section,
			"leftLowGearServoVal", 1700);
	highGearServoValLeft = config->Get<int> (config_section,
			"leftHighGearServoVal", 1050);
	lowGearServoValRight = config->Get<int> (config_section,
			"rightLowGearServoVal", 1100);
	highGearServoValRight = config->Get<int> (config_section,
			"rightHighGearServoVal", 1850);
	servoDisableDelay = config->Get<int> (config_section, "servoDisableDelay",
			5 * 50);
}

void Shifter::Output()
{
	//    DriverStation& station = *DriverStation::GetInstance();
	//    leftServo->Set(station.GetAnalogIn(1));
	//    rightServo->Set(station.GetAnalogIn(2));
	//    return;

	//Power down servos if robot is not moving for several seconds; governed by servoDisableTimer -dg
	if (servoDisableTimer > 0)
		servoDisableTimer--;

	//	const bool robotTryingToMove = (m_action_ptr.driveTrain->rate.rawForward != 0.0
	//			|| m_action_ptr.driveTrain->rate.rawTurn != 0.0);

	DriveTrainAction * dtptr = m_action->drivetrain;
	const bool robotTryingToMove = dtptr->raw.leftDutyCycle != 0.0
			|| dtptr->raw.rightDutyCycle != 0.0;

	if (robotTryingToMove || m_action->shifter->force)
		servoDisableTimer = servoDisableDelay; //reset timer

	bool enableServo = servoDisableTimer > 0;

	leftServo->SetEnabled(enableServo);
	rightServo->SetEnabled(enableServo);

	switch (m_action->shifter->gear)
	{
	case ACTION::GEARBOX::LOW_GEAR:
		leftServo->SetMicroseconds(lowGearServoValLeft);
		rightServo->SetMicroseconds(lowGearServoValRight);
		encoders.setHighGear(false);
		break;

	case ACTION::GEARBOX::HIGH_GEAR:
		leftServo->SetMicroseconds(highGearServoValLeft);
		rightServo->SetMicroseconds(highGearServoValRight);
		encoders.setHighGear(true);
		break;

	default:
		AsyncPrinter::Printf("Fatal: %s:%d\n", __FILE__, __LINE__);
		break;
	}
}

string Shifter::GetName()
{
	return "Shifter";
}

void Shifter::log()
{
	SmartDashboard::GetInstance()->PutString(
			"Robot Gear",
			(m_action->shifter->gear == ACTION::GEARBOX::HIGH_GEAR) ? "High Gear"
					: "Low Gear");
}
