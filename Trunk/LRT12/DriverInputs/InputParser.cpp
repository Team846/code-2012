#include "InputParser.h"
#include "../ActionData/ActionData.h"
#include "DebouncedJoystick.h"
#include "../Config/DriverStationConfig.h"

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
	m_action->launcher->speed = m_driver_stick->GetThrottle();
	m_action->launcher->state = ACTION::LAUNCHER::RUNNING;
//	if (m_operator_stick->IsButtonDown(SHOOT))
//		m_action->ballfeed->attemptToLoadRound = true;
	
	/***************** Conveyor **********************/
	m_action->ballfeed->feeder_state = ACTION::BALLFEED::FEEDING;

	/***************** Config **********************/
	if (m_driver_stick->IsButtonJustPressed(SAVE_CONFIG))
		m_action->config->save = true;
	if (m_driver_stick->IsButtonJustPressed(LOAD_CONFIG))
		m_action->config->load = true;
	if (m_driver_stick->IsButtonJustPressed(APPLY_CONFIG))
		m_action->config->apply = true;
}
