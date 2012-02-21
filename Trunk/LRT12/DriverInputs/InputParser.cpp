#include "InputParser.h"
#include "DebouncedJoystick.h"
#include "../Config/DriverStationConfig.h"
#include "../Util/AsyncPrinter.h"
#include "math.h"

#include "../ActionData/ActionData.h"
#include "../ActionData/DriveTrainAction.h"
#include "../ActionData/ShifterAction.h"
#include "../ActionData/LauncherAction.h"
#include "../ActionData/ConfigAction.h"
#include "../ActionData/BallFeedAction.h"

InputParser::InputParser()
{
	m_action = ActionData::GetInstance();
	m_driver_stick = new DebouncedJoystick(1,
			DriverStationConfig::NUM_JOYSTICK_BUTTONS,
			DriverStationConfig::NUM_JOYSTICK_AXES);
	m_operator_stick = new DebouncedJoystick(2,
			DriverStationConfig::NUM_JOYSTICK_BUTTONS,
			DriverStationConfig::NUM_JOYSTICK_AXES);
}

void InputParser::ProcessInputs()
{
	m_driver_stick->Update();
	m_operator_stick->Update();

	/***************** Shifter **********************/
	if (m_driver_stick->IsButtonJustPressed(SHIFT)) //Shift gear
	{
		AsyncPrinter::Printf("Shift button\n");
		if (m_action->shifter->gear == ACTION::GEARBOX::HIGH_GEAR)
			m_action->shifter->gear = ACTION::GEARBOX::LOW_GEAR;
		else
			m_action->shifter->gear = ACTION::GEARBOX::HIGH_GEAR;
	}

	/***************** Drivetrain **********************/
#define CLOSED_LOOP 0
#if CLOSED_LOOP
	m_action->drivetrain->rate.drive_control = true; //If driver control use velocity control
	m_action->drivetrain->rate.turn_control = true;
#else
	m_action->drivetrain->rate.drive_control = false; //If driver control use velocity control
	m_action->drivetrain->rate.turn_control = false;
#endif 
	m_action->drivetrain->position.drive_control = false;
	m_action->drivetrain->position.turn_control = false;

	m_action->drivetrain->rate.desiredDriveRate = -m_driver_stick->GetAxis(
			Joystick::kYAxis);
	m_action->drivetrain->rate.desiredTurnRate = -m_driver_stick->GetAxis(
			Joystick::kZAxis);

	/***************** Shooter **********************/
	//TODO SHOOT! 
//	m_action->launcher->speed += m_operator_stick->GetAxis(Joystick::kYAxis)*5;
	if (m_operator_stick->IsButtonJustPressed(DECREMENT_SPEED))
		m_action->launcher->speed -= 500;
	else if (m_operator_stick->IsButtonJustPressed(INCREMENT_SPEED))
		m_action->launcher->speed += 500;
		
	m_action->launcher->state = ACTION::LAUNCHER::RUNNING;

	/***************** Conveyor **********************/
	m_action->ballfeed->feeder_state = ACTION::BALLFEED::FEEDING;
	if (m_operator_stick->IsButtonDown(SHOOT))
		m_action->ballfeed->attemptToLoadRound = true;
	
	
	/***************** Ball Collector **********************/
	m_action->ballfeed->sweepArmOut = m_operator_stick->IsButtonDown(LOWER_CONVEYOR);
		
	/***************** Ball Feeder **********************/
	if (m_operator_stick->IsButtonDown(COLLECT_BALLS))
	{
		m_action->ballfeed->feeder_state = ACTION::BALLFEED::FEEDING;
	}
	else
	{
		m_action->ballfeed->feeder_state = ACTION::BALLFEED::HOLDING;
	}

	/***************** Config **********************/
	if (m_driver_stick->IsButtonJustPressed(SAVE_CONFIG))
		m_action->config->save = true;
	if (m_driver_stick->IsButtonJustPressed(LOAD_CONFIG))
		m_action->config->load = true;
	if (m_driver_stick->IsButtonJustPressed(APPLY_CONFIG))
		m_action->config->apply = true;
}
