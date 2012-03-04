#include "InputParser.h"
#include "DebouncedJoystick.h"
#include "../Config/DriverStationConfig.h"
#include "../Util/AsyncPrinter.h"
#include "math.h"

#include "../ActionData/ActionData.h"
#include "../ActionData/AutonomousAction.h"
#include "../ActionData/DriveTrainAction.h"
#include "../ActionData/ShifterAction.h"
#include "../ActionData/LauncherAction.h"
#include "../ActionData/ConfigAction.h"
#include "../ActionData/BallFeedAction.h"
#include "../ActionData/BPDAction.h"

InputParser::InputParser()
{
	m_action_ptr = ActionData::GetInstance();
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
		if (m_action_ptr->shifter->gear == ACTION::GEARBOX::HIGH_GEAR)
			m_action_ptr->shifter->gear = ACTION::GEARBOX::LOW_GEAR;
		else
			m_action_ptr->shifter->gear = ACTION::GEARBOX::HIGH_GEAR;
	}

	/***************** Ball Feeder **********************/
	if (m_operator_stick->IsButtonDown(COLLECT_BALLS))
	{
		m_action_ptr->ballfeed->feeder_state = ACTION::BALLFEED::FEEDING;
	}
	else
	{
		m_action_ptr->ballfeed->feeder_state = ACTION::BALLFEED::HOLDING;
	}

	/***************** Drive assistance ****************/
	if (m_driver_stick->IsButtonDown(KEYTRACK))
	{
		if (m_action_ptr->auton->state != ACTION::AUTONOMOUS::AUTOALIGN
				&& m_action_ptr->auton->completion_status != ACTION::SUCCESS)
		{
			m_action_ptr->auton->state = ACTION::AUTONOMOUS::KEYTRACK;
		}
	}
	//	else if (m_driver_stick->IsButtonDown(BRIDGEBALANCE))
	//	{
	//		m_action_ptr->auton->state = ACTION::AUTONOMOUS::BRIDGEBALANCE;
	//	}
	else
	{
		m_action_ptr->auton->state = ACTION::AUTONOMOUS::TELEOP;
	}

	/***************** Drivetrain **********************/
	if (m_action_ptr->auton->state == ACTION::AUTONOMOUS::TELEOP)
	{
		if (m_driver_stick->IsButtonDown(9))
		{
			m_action_ptr->drivetrain->rate.drive_control = true;
			m_action_ptr->drivetrain->rate.turn_control = true;

			m_action_ptr->drivetrain->position.drive_control = true;
			m_action_ptr->drivetrain->position.turn_control = true;

			static bool temp = true;
			if (m_driver_stick->IsButtonDown(10) || temp)
			{
				temp = false;
				m_action_ptr->drivetrain->position.reset_translate_zero = true;
			}
			m_action_ptr->drivetrain->position.desiredRelativeDrivePosition
					= -m_driver_stick->GetAxis(Joystick::kYAxis) * 12 * 4; //inches
		}
		else
		{
#define CLOSED_LOOP 1
#if CLOSED_LOOP
			m_action_ptr->drivetrain->rate.drive_control = true; //If driver control use velocity control
			m_action_ptr->drivetrain->rate.turn_control = true;
#else
			m_action_ptr->drivetrain->rate.drive_control = false; //If driver control use velocity control
			m_action_ptr->drivetrain->rate.turn_control = false;
#endif 
			//		m_action_ptr->drivetrain->position.drive_control = false;
			//		m_action_ptr->drivetrain->position.turn_control = false;
			m_action_ptr->drivetrain->position.drive_control = false;
			m_action_ptr->drivetrain->position.turn_control = false;

			m_action_ptr->drivetrain->rate.desiredDriveRate = pow(
					-m_driver_stick->GetAxis(Joystick::kYAxis), 1);
			m_action_ptr->drivetrain->rate.desiredTurnRate = pow(
					-m_driver_stick->GetAxis(Joystick::kZAxis), 3);
		}

	}

	/***************** Launcher **********************/
	if (m_operator_stick->IsButtonJustPressed(TRAJECTORY_UP))
	{
		m_action_ptr->launcher->topTrajectory = true;
	}
	else if (m_operator_stick->IsButtonJustPressed(TRAJECTORY_DOWN))
	{
		m_action_ptr->launcher->topTrajectory = false;
	}

	if (m_operator_stick->IsButtonDown(LOW_SPEED))
	{
		m_action_ptr->launcher->desiredSpeed = ACTION::LAUNCHER::SLOW;
	}
	else if (m_operator_stick->IsButtonDown(HIGH_SPEED))
	{
		m_action_ptr->launcher->desiredSpeed = ACTION::LAUNCHER::FASTEST;
	}
	else
	{
		m_action_ptr->launcher->desiredSpeed = ACTION::LAUNCHER::MEDIUM;
	}

	m_action_ptr->launcher->state = ACTION::LAUNCHER::RUNNING;

	/***************** Conveyor **********************/

	if (m_operator_stick->IsButtonDown(SHOOT))
	{
		m_action_ptr->ballfeed->attemptToLoadRound = true;
	}
	else
	{
		m_action_ptr->ballfeed->attemptToLoadRound = false;
	}

	/***************** WEDGE **********************/
	if (m_operator_stick->IsButtonJustPressed(WEDGE_UP)
			|| m_action_ptr->ballfeed->sweepArmOut)
	{
		m_action_ptr->wedge->state = ACTION::WEDGE::PRESET_TOP;
	}
	else if (m_operator_stick->IsButtonJustPressed(WEDGE_DOWN))
	{
		m_action_ptr->wedge->state = ACTION::WEDGE::PRESET_BOTTOM;
	}

	/***************** Ball Collector **********************/
	if (!(m_action_ptr->wedge->state == ACTION::WEDGE::PRESET_BOTTOM))
	{
		m_action_ptr->ballfeed->sweepArmOut = m_driver_stick->IsButtonDown(
				LOWER_COLLECTOR);
		if (m_driver_stick->IsButtonDown(LOWER_COLLECTOR))
		{
			m_action_ptr->ballfeed->feeder_state = ACTION::BALLFEED::FEEDING;
		}
	}
	else
	{
		m_action_ptr->ballfeed->sweepArmOut = false;
	}

	if (m_operator_stick->IsButtonDown(PURGE))
	{
		m_action_ptr->ballfeed->feeder_state = ACTION::BALLFEED::PURGING;
		m_action_ptr->launcher->desiredSpeed = ACTION::LAUNCHER::SLOW;
	}

	/***************** Config **********************/
	if (m_driver_stick->IsButtonJustPressed(SAVE_CONFIG))
		m_action_ptr->config->save = true;
	if (m_driver_stick->IsButtonJustPressed(LOAD_CONFIG))
		m_action_ptr->config->load = true;
	if (m_driver_stick->IsButtonJustPressed(APPLY_CONFIG))
		m_action_ptr->config->apply = true;

}
