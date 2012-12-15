#include "InputParser.h"
#include "DebouncedJoystick.h"
#include "../Config/DriverStationConfig.h"
#include "../Util/AsyncPrinter.h"
//#include "math.h"
#include <cmath>

#include "../ActionData/ActionData.h"
#include "../ActionData/AutonomousAction.h"
#include "../ActionData/DriveTrainAction.h"
#include "../ActionData/ShifterAction.h"
#include "../ActionData/LauncherAction.h"
#include "../ActionData/ConfigAction.h"
#include "../ActionData/BallFeedAction.h"
#include "../ActionData/BPDAction.h"

#define WHEEL 1

InputParser::InputParser()
{
	m_action_ptr = ActionData::GetInstance();
	m_driver_stick = new DebouncedJoystick(1,
			DriverStationConfig::NUM_JOYSTICK_BUTTONS,
			DriverStationConfig::NUM_JOYSTICK_AXES);
	m_operator_stick = new DebouncedJoystick(3,
			DriverStationConfig::NUM_JOYSTICK_BUTTONS,
			DriverStationConfig::NUM_JOYSTICK_AXES);
	m_driver_steering_wheel = new DebouncedJoystick(2, 12, 2);
	m_use_steering_wheel = 0;
}

void InputParser::ProcessInputs()
{
	m_driver_stick->Update();
	m_operator_stick->Update();

	/***************** Shifter **********************/
	if (m_driver_stick->IsButtonDown(SHIFT)) //Shift gear
	{
		m_action_ptr->shifter->gear = ACTION::GEARBOX::LOW_GEAR;
	}
	else
	{
		m_action_ptr->shifter->gear = ACTION::GEARBOX::HIGH_GEAR;
	}

	/***************** Ball Feeder **********************/
	if (m_operator_stick->IsButtonDown(COLLECT_BALLS))
	{
		m_action_ptr->ballfeed->feeder_state = ACTION::BALLFEED::FEEDING;
	}
	else if (m_action_ptr->wedge->state == ACTION::WEDGE::PRESET_BOTTOM)
	{
		m_action_ptr->ballfeed->feeder_state = ACTION::BALLFEED::FREEZING;
	}
	else
	{
		m_action_ptr->ballfeed->feeder_state = ACTION::BALLFEED::HOLDING;
	}
	static bool hasBeenReleased = true;
	/***************** Drive assistance ****************/
	if (m_driver_stick->IsButtonDown(AUTOAIM))
	{
		if (hasBeenReleased)
		{
			m_action_ptr->auton->state = ACTION::AUTONOMOUS::AUTOALIGN;
		}
		hasBeenReleased = false;
	}
	else if (m_driver_stick->IsButtonDown(KEYTRACK))
	{
		m_action_ptr->ballfeed->feeder_state = ACTION::BALLFEED::FEEDING;
		m_action_ptr->auton->state = ACTION::AUTONOMOUS::KEYTRACK;
		m_action_ptr->drivetrain->rate.desiredDriveRate = pow(
				-m_driver_stick->GetAxis(Joystick::kYAxis), 1);
	}
	else if (m_driver_stick->IsButtonDown(POSITION_HOLD))
	{
		m_action_ptr->auton->state = ACTION::AUTONOMOUS::POSITION_HOLD;
	}
	else
	{
		hasBeenReleased = true;
		m_action_ptr->auton->state = ACTION::AUTONOMOUS::TELEOP;
	}

	if (m_driver_stick->IsButtonDown(AUTO_AIM_DIR))
	{
		m_action_ptr->auton->turnDir = ACTION::AUTONOMOUS::COUNTER_CLOCKWISE;
	}
	else
	{
		m_action_ptr->auton->turnDir = ACTION::AUTONOMOUS::CLOCKWISE;
	}

	/***************** Drivetrain **********************/
	if (m_action_ptr->auton->state == ACTION::AUTONOMOUS::TELEOP)
	{
		if (m_driver_stick->IsButtonJustPressed(WHEEL_TOGGLE))
			m_use_steering_wheel = !m_use_steering_wheel;

		if (m_driver_stick->IsButtonDown(RESET_ZERO))
		{
			m_action_ptr->drivetrain->position.reset_translate_zero = true;
			m_action_ptr->drivetrain->position.reset_turn_zero = true;
		}
		else
		{
#define TURN_EXPONENT 3		//3	(Anurag 10/18/12)
			double turn = 0.0;
			if (m_use_steering_wheel)
				turn = -m_driver_steering_wheel->GetAxis(Joystick::kXAxis);
			else //no wheel
				turn = -m_driver_stick->GetAxis(Joystick::kZAxis);
			turn = pow(turn, TURN_EXPONENT);
			
			double forward = pow(-m_driver_stick->GetAxis(Joystick::kYAxis), 1);

			double turnComposite = 0.0;

			double absForward = abs(forward);
			double blend = (1 - absForward); // always between 0 and 1
			blend = pow(blend, 2);

			turn *= (1); //scale turn rate

			const double turnInPlace = turn;
			const double turnConstantRadius = turn * absForward;
			turnComposite = turnInPlace * (blend) + turnConstantRadius * (1
					- blend);

			m_action_ptr->drivetrain->rate.desiredDriveRate = forward;
			m_action_ptr->drivetrain->rate.desiredTurnRate = turnComposite;
		}
	}

	/***************** Launcher **********************/
	if (m_operator_stick->IsButtonJustPressed(FENDER_SHOT_SELECT))
	{
		m_action_ptr->launcher->isFenderShot = true;
	}
	else if (m_operator_stick->IsButtonJustPressed(KEY_SHOT_SELECT))
	{
		m_action_ptr->launcher->isFenderShot = false;
	}

	if (m_operator_stick->IsButtonDown(LOWER_SHOT))
	{
		if (m_action_ptr->launcher->isFenderShot)
		{
			m_action_ptr->launcher->desiredTarget
					= ACTION::LAUNCHER::FENDER_SHOT_LOW;
		}
		else
		{
			m_action_ptr->launcher->desiredTarget
					= ACTION::LAUNCHER::KEY_SHOT_LOW;
		}
	}
	else if (m_operator_stick->IsButtonDown(HIGH_SPEED))
	{
		if (m_action_ptr->launcher->isFenderShot)
		{
			m_action_ptr->launcher->desiredTarget
					= ACTION::LAUNCHER::FAR_FENDER_SHOT_HIGH;
		}
		else
		{
			m_action_ptr->launcher->desiredTarget
					= ACTION::LAUNCHER::KEY_SHOT_HIGH;
		}
	}
	else
	{
		if (m_action_ptr->launcher->isFenderShot)
		{
			m_action_ptr->launcher->desiredTarget
					= ACTION::LAUNCHER::FENDER_SHOT_HIGH;
		}
		else
		{
			m_action_ptr->launcher->desiredTarget
					= ACTION::LAUNCHER::KEY_SHOT_HIGH;
		}
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
		m_action_ptr->wedge->try_again = true;
	}
	else if (m_operator_stick->IsButtonJustPressed(WEDGE_DOWN))
	{
		m_action_ptr->wedge->state = ACTION::WEDGE::PRESET_BOTTOM;
		m_action_ptr->wedge->try_again = true;
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
		//		m_action_ptr->launcher->desiredTarget
		//				= ACTION::LAUNCHER::FENDER_SHOT_HIGH;
	}

	/***************** Config **********************/
	if (m_driver_stick->IsButtonJustPressed(SAVE_CONFIG))
		m_action_ptr->config->save = true;
	if (m_driver_stick->IsButtonJustPressed(LOAD_CONFIG))
		m_action_ptr->config->load = true;
	if (m_driver_stick->IsButtonJustPressed(APPLY_CONFIG))
		m_action_ptr->config->apply = true;

}
