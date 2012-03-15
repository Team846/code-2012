/*
 * AutonomousFunctions.cpp
 *
 *  Created on: Feb 28, 2012
 *      Author: rbtying
 */

#include "AutonomousFunctions.h"
#include "../ActionData/LauncherAction.h"
#include "../ActionData/BallFeedAction.h"
#include "../ActionData/BPDAction.h"
#include "../Util/AsyncPrinter.h"
#include "../Sensors/DriveEncoders.h"
#include "../Util/PID.h"
#include <string>

#define TIMEOUT_ENABLED 0

AutonomousFunctions* AutonomousFunctions::m_instance = NULL;

AutonomousFunctions::AutonomousFunctions() :
	Configurable(), Loggable(), m_name("AutonomousFunctions")
{
	m_task = new Task("AutonFuncTask",
			(FUNCPTR) AutonomousFunctions::taskEntryPoint,
			Task::kDefaultPriority - 1);
	m_task_sem = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
	m_is_running = false;
	m_action = ActionData::GetInstance();

	bridgeTipState = 0;
	m_haf_cyc_delay = RobotConfig::LOOP_RATE / 2;
	m_adj_cyc_delay = 0;
	m_align_setpoint = 0;
	Configure();
	m_action->imu->pitch = 0.0;
}

AutonomousFunctions::~AutonomousFunctions()
{
	stopBackgroundTask();

	delete m_task;

	int error = semDelete(m_task_sem);
	if (error)
	{
		printf("SemDelete Error=%d\n", error);
	}
}

void AutonomousFunctions::taskEntryPoint()
{
	getInstance()->task();
}

void AutonomousFunctions::startBackgroundTask()
{
	m_is_running = true;
	m_task->Start();
}

void AutonomousFunctions::stopBackgroundTask()
{
	if (m_is_running)
	{
		m_is_running = false;
		UINT32 task_id = m_task->GetID();
		m_task->Stop();
		printf("Task 0x%x killed for AutonomousFunctions\n", task_id);
	}
}

void AutonomousFunctions::releaseSemaphore()
{
	semGive(m_task_sem);
}

void AutonomousFunctions::task()
{
	while (m_is_running)
	{
		semTake(m_task_sem, WAIT_FOREVER);
		if (!m_is_running)
		{
			break;
		}

		if (m_action->auton->state != m_last_state)
		{
			if (m_action->auton->state != ACTION::AUTONOMOUS::TELEOP)
			{
				m_action->auton->completion_status = ACTION::IN_PROGRESS;
				m_action->shifter->gear = ACTION::GEARBOX::LOW_GEAR;
			}
			switch (m_action->auton->state)
			{
			case ACTION::AUTONOMOUS::TELEOP:
				m_hit_key_flag = false;
				break;
			case ACTION::AUTONOMOUS::BRIDGEBALANCE:
				m_counter = m_timeout_bridgebalance;
				m_bridgebalance_pid.reset();
				break;
			case ACTION::AUTONOMOUS::KEYTRACK:
				m_counter = m_timeout_keytrack;
				break;
			case ACTION::AUTONOMOUS::AUTOALIGN:
				m_counter = m_timeout_autoalign;
				break;
			case ACTION::AUTONOMOUS::AUTON_MODE:
				loadQueue();
				break;
			}
		}
		m_last_state = m_action->auton->state;

		// timeout
#if TIMEOUT_ENABLED
		if (--m_counter <= 0)
		{
			m_action->auton->state = ACTION::AUTONOMOUS::TELEOP;
			m_action->auton->completion_status = ACTION::FAILURE;
		}
#endif

		switch (m_action->auton->state)
		{
		case ACTION::AUTONOMOUS::TELEOP:
			bridgeTipState = 0;
			m_haf_cyc_delay = RobotConfig::LOOP_RATE / 2;
			m_adj_cyc_delay = M_CYCLES_TO_DELAY;
#define CLOSED_LOOP 1
#if CLOSED_LOOP
			m_action->drivetrain->rate.drive_control = true; //If driver control use velocity control
			m_action->drivetrain->rate.turn_control = true;
#else
			m_action->drivetrain->rate.drive_control = false; //If driver control use velocity control
			m_action->drivetrain->rate.turn_control = false;
#endif 
			m_action->drivetrain->position.drive_control = false;
			m_action->drivetrain->position.turn_control = false;
			break;
		case ACTION::AUTONOMOUS::BRIDGEBALANCE:
			if (bridgeBalance())
			{
				m_action->auton->completion_status = ACTION::SUCCESS;
				//				m_action->auton->state = ACTION::AUTONOMOUS::TELEOP;
			}
			else
				m_action->auton->completion_status = ACTION::IN_PROGRESS;
			break;
		case ACTION::AUTONOMOUS::KEYTRACK:
			if (keyTrack())
			{
				m_action->auton->completion_status = ACTION::SUCCESS;
				m_action->auton->state = ACTION::AUTONOMOUS::AUTOALIGN;
			}
			else
				m_action->auton->completion_status = ACTION::IN_PROGRESS;
			break;
		case ACTION::AUTONOMOUS::AUTOALIGN:
			if (autoAlign())
			{
				m_action->auton->completion_status = ACTION::SUCCESS;
				//				m_action->auton->state = ACTION::AUTONOMOUS::TELEOP;
			}
			else
				m_action->auton->completion_status = ACTION::IN_PROGRESS;
			break;
		case ACTION::AUTONOMOUS::AUTON_MODE:
			if (autonomousMode())
			{
				m_action->auton->completion_status = ACTION::SUCCESS;
				m_action->auton->state = ACTION::AUTONOMOUS::TELEOP;
			}
			else
			{
				m_action->auton->completion_status = ACTION::IN_PROGRESS;
			}
			break;
		}
	}
}

bool AutonomousFunctions::otherBridgeBalance(){
	
}
bool AutonomousFunctions::anotherBridgeBalance()
{
	static int e = 0;
	static bool hasStartedTipping = false;
	
	static bool after_process = false;
	static int aftercounter = 0;
	
	if (++e % 25 == 0)
		AsyncPrinter::Printf("Balancing\n");
	
	if (!hasStartedTipping)
	{
		if (++e % 5 == 0)
			AsyncPrinter::Printf("Waiting for tip\n");
		m_direction = Util::Sign<double>(m_action->imu->pitch);

		m_action->drivetrain->position.drive_control = false;
		m_action->drivetrain->position.turn_control = false;
		m_action->drivetrain->rate.drive_control = true;
		m_action->drivetrain->rate.turn_control = true;

		m_action->drivetrain->rate.desiredDriveRate = 0.11 * m_direction;
		m_action->drivetrain->rate.desiredTurnRate = 0.0;

		static int prev_side = 0; // -1 neg, 0 flat, 1 pos
		int side = 0;
		if (fabs(m_action->imu->pitch) > 12)
		{
			side = m_direction;
		}
		else
		{
			side = 0;
		}

		if (side == 0 && prev_side != side)
		{
			AsyncPrinter::Printf("We have tipped\n");
			//hasStartedTipping = true;
		
			after_process = true;
			
			/* changes begin */
			
			m_action->drivetrain->rate.desiredDriveRate = -0.1 * m_direction;
			m_action->drivetrain->rate.desiredTurnRate = 0.0;
			
			/* changes end */
		}
		prev_side = side;
	}
	else if(after_process)
	{
		AsyncPrinter::Printf("Going back...");
		
		m_action->drivetrain->rate.desiredDriveRate = 0.1 * m_direction;
		m_action->drivetrain->rate.desiredTurnRate = 0.0;
					
		if(++aftercounter == 25)
		{
			m_action->drivetrain->rate.desiredDriveRate = 0.0;
			m_action->drivetrain->rate.desiredTurnRate = 0.0;
		}
	}
	else
	{
		if (++e % 5 == 0)
			AsyncPrinter::Printf("Stopping \n");
		m_action->drivetrain->position.drive_control = false;
		m_action->drivetrain->position.turn_control = false;
		m_action->drivetrain->rate.drive_control = false;
		m_action->drivetrain->rate.turn_control = false;
		m_action->drivetrain->rate.desiredDriveRate = 0.0;
		m_action->drivetrain->rate.desiredTurnRate = 0.0;
		
		if (!m_action->drivetrain->position.drive_control)
		{
			AsyncPrinter::Printf("setting\n");
			m_action->drivetrain->position.absoluteTranslate = false;
			m_action->drivetrain->position.drive_control = false;
			m_action->drivetrain->position.turn_control = false;
			m_action->drivetrain->rate.drive_control = false;
			m_action->drivetrain->rate.turn_control = false;
			m_action->drivetrain->position.desiredRelativeDrivePosition = 0.0;
			//		m_action->drivetrain->position.desiredRelativeDrivePosition = -120; //TODO Check me
			m_action->drivetrain->position.desiredAbsoluteTurnPosition = 0;
		}
		else
		{
			if (m_action->drivetrain->previousDriveOperationComplete)
				return true;
		}
	}

	return false;

	//	m_bridgebalance_pid.setSetpoint(m_bridgebalance_setpoint);
	m_bridgebalance_pid.setSetpoint(0);
	m_bridgebalance_pid.setInput(m_action->imu->pitch);
	m_bridgebalance_pid.update(1.0 / RobotConfig::LOOP_RATE);

	// drive on rate control, turn on rate control
	m_action->drivetrain->rate.drive_control = true;
	m_action->drivetrain->rate.turn_control = true;
	m_action->drivetrain->position.drive_control = false;
	m_action->drivetrain->position.turn_control = false;

	// set drivetrain and switch sign
	//	m_action->drivetrain->position.desiredRelativeDrivePosition
	//			= -m_bridgebalance_pid.getOutput();
	m_action->drivetrain->rate.desiredDriveRate = Util::Clamp<double>(
			-m_bridgebalance_pid.getOutput(), -0.1, 0.1);

	//	if (++e % 25 == 0)
	//		AsyncPrinter::Printf("Bridge Out %.4f\n",
	//				m_bridgebalance_pid.getOutput());
	//	m_action->drivetrain->rate.desiredDriveRate
	//			= m_bridgebalance_pid.getOutput();
	m_action->drivetrain->rate.desiredTurnRate = 0.0;

	if (fabs(m_bridgebalance_pid.getError()) < m_bridgebalance_threshold)
	{
		//			m_action->auton->state = ACTION::AUTONOMOUS::TELEOP;
		return true;//we're done
		//		m_action->drivetrain->rate.drive_control = true;
		//		m_action->drivetrain->rate.turn_control = true;
		//		m_action->drivetrain->position.drive_control = false;
		//		m_action->drivetrain->position.turn_control = false;
		//		m_action->drivetrain->rate.desiredDriveRate = 0.0;
		//		m_action ->drivetrain->rate.desiredTurnRate = 0.0;
	}
	return false;
}

bool AutonomousFunctions::alternateBridgeBalance()
{
	m_action->drivetrain->rate.drive_control = true;
	m_action->drivetrain->rate.turn_control = true;
	m_action->drivetrain->position.drive_control = false;
	m_action->drivetrain->position.turn_control = false;

	if (m_action->imu->pitch > m_bridgebalance_setpoint)
	{
		m_action->drivetrain->rate.desiredDriveRate = m_keytrack_forward_rate;
	}
	else
	{
		m_action->drivetrain->rate.desiredDriveRate = -m_keytrack_forward_rate;
	}

	if (fabs(m_action->imu->pitch - m_bridgebalance_setpoint)
			< m_bridgebalance_threshold)
	{
		m_action->drivetrain->rate.desiredDriveRate = 0.0;
		m_action ->drivetrain->rate.desiredTurnRate = 0.0;
		return true;//we're done
	}
	return false;

}


bool AutonomousFunctions::bridgeBalance()
{
	static int e = 0;
	if (++e % 25 == 0)
		AsyncPrinter::Printf("Balancing\n");
	
	static int cyclecount = 0;
	cyclecount++;
	int side = 0;
	switch (bridgeTipState)
	{
	case 0:
		if (++e % 5 == 0)
			AsyncPrinter::Printf("Waiting for tip\n");
		m_direction = Util::Sign<double>(m_action->imu->pitch);

		m_action->drivetrain->position.drive_control = false;
		m_action->drivetrain->position.turn_control = false;
		m_action->drivetrain->rate.drive_control = true;
		m_action->drivetrain->rate.turn_control = true;

		m_action->drivetrain->rate.desiredDriveRate = 0.11 * m_direction;
		m_action->drivetrain->rate.desiredTurnRate = 0.0;

		static int prev_side = 0; // -1 neg, 0 flat, 1 pos
		if (fabs(m_action->imu->pitch) > 12)
		{
			side = m_direction;
		}
		else
		{
			side = 0;
		}

		if (side == 0 && prev_side != side)
		{
			AsyncPrinter::Printf("We have tipped\n");
			cyclecount = 0;
			bridgeTipState++;
			firstPlace = DriveEncoders::GetInstance().getRobotDist();
		}
		prev_side = side;
		break;
	case 1:
		m_bridgebalance_pid.setInput(m_action->imu->pitch);
		m_bridgebalance_pid.setSetpoint(0.0);
		if (e % 2 == 0)
		{
			m_bridgebalance_pid.update(2.0 / RobotConfig::LOOP_RATE);
		}
		
		m_action->drivetrain->position.drive_control = false;
		m_action->drivetrain->position.turn_control = false;
		m_action->drivetrain->rate.drive_control = true;
		m_action->drivetrain->rate.turn_control = true;

		m_action->drivetrain->rate.desiredDriveRate = Util::Clamp<double>(m_bridgebalance_pid.getOutput(), -0.25, 0.25);
		if (e % 4 == 0)
		{
			AsyncPrinter::Printf("Error %.2f, diff %.2f\n", m_bridgebalance_pid.getError(), (m_action->imu->pitch - lastPitch)* 25.0);
		}
		m_action->drivetrain->rate.desiredTurnRate = 0.0;
		break;
//	case 1:
//		m_action->drivetrain->position.drive_control = false;
//		m_action->drivetrain->position.turn_control = false;
//		m_action->drivetrain->rate.drive_control = true;
//		m_action->drivetrain->rate.turn_control = true;
//
//		m_action->drivetrain->rate.desiredDriveRate = -0.11 * m_direction;
//		m_action->drivetrain->rate.desiredTurnRate = 0.0;
//		if (fabs(m_action->imu->pitch) < 0.5 || fabs(m_action->imu->pitch) > fabs(lastPitch))
//		{
//			AsyncPrinter::Printf("Detected\n");
//			bridgeTipState++;
//			secondPlace = DriveEncoders::GetInstance().getRobotDist();
//		}
//		break;
//	case 2:
//		m_action->drivetrain->position.absoluteTranslate = false;
//		m_action->drivetrain->position.absoluteTurn = false;
//
//		m_action->drivetrain->position.drive_control = true;
//		m_action->drivetrain->position.turn_control = false;
//		m_action->drivetrain->rate.drive_control = true;
//		m_action->drivetrain->rate.turn_control = true;
//		
//		double diff = fabs(firstPlace - secondPlace);
//		AsyncPrinter::Printf("Diff %.2f\n", diff);
//		m_action->drivetrain->position.desiredRelativeDrivePosition = (diff + 2.0) * m_direction;
//		m_action->drivetrain->rate.desiredTurnRate = 0.0;
//		bridgeTipState++;
//		break;
//	case 3:
//		break;
	default:
		bridgeTipState = 0;
		break;
	}

	lastPitch = m_action->imu->pitch;
	return false;
}

bool AutonomousFunctions::keyTrack()
{
	m_action->drivetrain->rate.drive_control = true;
	m_action->drivetrain->rate.turn_control = true;
	m_action->drivetrain->position.drive_control = false;
	m_action->drivetrain->position.turn_control = false;
	m_action->drivetrain->rate.desiredDriveRate = m_keytrack_forward_rate;
	m_action->drivetrain->rate.desiredTurnRate = 0.0;

	bool state = m_action->cam->key.higher >= m_keytrack_threshold;
	m_hit_key_flag |= state;

	if (m_hit_key_flag)
	{
		if (!state)
		{
			m_hit_key_flag = false;
			m_action->drivetrain->rate.desiredDriveRate = 0.0;
			m_action ->drivetrain->rate.desiredTurnRate = 0.0;
			AsyncPrinter::Printf("Done Keytrack Done\n");
			return true;

		}
	}
	return false;
}

bool AutonomousFunctions::autoAlign()
{
	m_auto_aim_pid.setSetpoint(m_align_setpoint);
	if (m_action->cam->align.status != ACTION::CAMERA::NO_TARGET)
	{
		double input = m_action->cam->align.arbitraryOffsetFromUDP;
		m_auto_aim_pid.setInput(input / 127.0);
		m_auto_aim_pid.update(1.0 / RobotConfig::LOOP_RATE);
		double out = m_auto_aim_pid.getOutput();
		if (fabs(out) > m_max_align_turn_rate)
		{
			out = Util::Sign<double>(out) * m_max_align_turn_rate;
		}

		m_action->drivetrain->rate.drive_control = true;
		m_action->drivetrain->rate.turn_control = true;
		m_action->drivetrain->position.drive_control = false;
		m_action->drivetrain->position.turn_control = false;

		// switch directions depending on error

		if (fabs(m_auto_aim_pid.getError()) <= m_align_threshold)
		{
//			m_align_turned = out;
			
			m_action->drivetrain->rate.desiredTurnRate = 0.0;
			m_action->drivetrain->rate.desiredDriveRate = 0.0;
			AsyncPrinter::Printf("Aiming done\n");
			return true;
		}
		else
		{
			if (fabs(out) <= m_min_align_turn_rate)
			{
				m_action->drivetrain->rate.desiredTurnRate
						= Util::Sign<double>(out) * m_min_align_turn_rate;
			}
			else
			{
				m_action->drivetrain->rate.desiredTurnRate = out;
			}
			m_action->drivetrain->rate.desiredDriveRate = 0.0;
		}
	}
	return false;
}

#define BALLS_TO_SHOOT 2
#define DEBUG 1
bool AutonomousFunctions::autonomousMode()
{

#if DEBUG
	static int e = 0;
	if (++e % 40 == 0)
		AsyncPrinter::Printf("Entering %s\r\n",
				getAutonomousStageName(m_curr_auton_stage).c_str());
#endif
	switch (m_curr_auton_stage)
	{
	case INIT:
#warning Set speed correctly and check trajectory 
		m_action->launcher->desiredTarget = ACTION::LAUNCHER::KEY_SHOT_HIGH;
		m_action->drivetrain->position.reset_turn_zero = true;
		M_CYCLES_TO_DELAY
				= static_cast<int> ((DriverStation::GetInstance()->GetAnalogIn(
						1) / 5.0) * 10 * RobotConfig::LOOP_RATE);
		m_adj_cyc_delay = M_CYCLES_TO_DELAY;
		advanceQueue();
		break;
	case KEY_TRACK:
		if (keyTrack())
		{
			advanceQueue();
		}
		break;
	case AIM:
		if (autoAlign())
		{
			advanceQueue();
		}
		break;
	case SHOOT:
		AsyncPrinter::Printf("Pretend Shooting\n");
		
		advanceQueue();
		break;
		if (autoAlign() && m_action->launcher->ballLaunchCounter
				<= BALLS_TO_SHOOT)
		{
			m_action->ballfeed->attemptToLoadRound = true;
		}
		else
		{
			advanceQueue();
		}
		break;
	case MOVE_BACK_INIT:
#warning Verify amount to drive back
		m_action->wedge->state = ACTION::WEDGE::PRESET_BOTTOM;
		m_action->drivetrain->position.absoluteTranslate = false;
		m_action->drivetrain->position.absoluteTurn = false;
		m_action->drivetrain->position.drive_control = true;
		m_action->drivetrain->position.turn_control = false;
		
		m_action->drivetrain->position.desiredRelativeDrivePosition = -12; //TODO Check me
		//		m_action->drivetrain->position.desiredRelativeDrivePosition = -120; //TODO Check me
		m_action->drivetrain->position.desiredRelativeTurnPosition = 0;
		advanceQueue();
		break;
	case MOVE_TO_FENDER_INIT:
#warning Verify amount to drive forward
		m_action->drivetrain->position.absoluteTranslate = false;
		m_action->drivetrain->position.absoluteTurn = false;
		m_action->drivetrain->position.drive_control = true;
		m_action->drivetrain->position.turn_control = false;
		
		m_action->drivetrain->position.desiredRelativeDrivePosition = 24; //TODO Check me
		//		m_action->drivetrain->position.desiredRelativeDrivePosition = -120; //TODO Check me
		m_action->drivetrain->position.desiredRelativeTurnPosition = 0;
		advanceQueue();
		break;
	case DROP_WEDGE:
		m_action->ballfeed->feeder_state = ACTION::BALLFEED::FREEZING;
		m_action->wedge->state = ACTION::WEDGE::PRESET_BOTTOM;
		advanceQueue();
		break;
	case RAISE_WEDGE:
		m_action->wedge->state = ACTION::WEDGE::PRESET_TOP;
		m_action->ballfeed->feeder_state = ACTION::BALLFEED::HOLDING;
		advanceQueue();
		break;
	case DELAY_HALF_SEC:
		if (--m_haf_cyc_delay <= 0)
		{
			m_haf_cyc_delay = RobotConfig::LOOP_RATE / 2;
			advanceQueue();
		}
		break;
	case ADJUSTABLE_DELAY:
		if (--m_adj_cyc_delay <= 0)
		{
			m_adj_cyc_delay = M_CYCLES_TO_DELAY;
			advanceQueue();
		}
		break;
	case WAIT_FOR_POSITION:

		if (m_action->drivetrain->previousDriveOperationComplete)
		{
			advanceQueue();
		}

		break;
	case DONE:
#if DEBUG
		AsyncPrinter::Printf("Finished\r\n");
#endif
		return true;
		break;
	}
	return false;
}

AutonomousFunctions * AutonomousFunctions::getInstance()
{
	if (m_instance == NULL)
	{
		m_instance = new AutonomousFunctions();
	}
	return m_instance;
}

void AutonomousFunctions::Configure()
{
	Config * c = Config::GetInstance();
	m_timeout_bridgebalance = c->Get<int> (m_name, "bridgeTimeout", 100);
	m_timeout_keytrack = c->Get<int> (m_name, "keyTimeout", 100);
	m_timeout_autoalign = c->Get<int> (m_name, "alignTimeout", 100);

	double p, i, d;
	p = c->Get<double> (m_name, "bridgeP", 0.05);
	i = c->Get<double> (m_name, "bridgeI", 0.0);
	d = c->Get<double> (m_name, "bridgeD", 0.0);
	m_bridgebalance_pid.setParameters(p, i, d, 0, 0.87, false);
	m_bridgebalance_setpoint = c->Get<double> (m_name, "bridgeSetpoint", 00);
	m_bridgebalance_threshold = c->Get<double> (m_name, "bridgeThreshold", 1);
	m_bridgebalance_angular_rate_threshold = c->Get<double> (m_name,
			"bridgeAngularRateThreshold", 30);

	m_keytrack_forward_rate = c->Get<double> (m_name, "keyForwardRate", 0.1);
	m_keytrack_threshold = c->Get<double> (m_name, "keyThreshold", 127);

	p = c->Get<double> (m_name, "autoAlignP", 32.0);
	i = c->Get<double> (m_name, "autoAlignI", 12.7);
	d = c->Get<double> (m_name, "autoAlignD", 0.0);
	m_auto_aim_pid.setParameters(p, i, d, 0.1, false);
	m_max_align_turn_rate = c->Get<double> (m_name, "alignMaxTurnRate", 0.15);
	m_min_align_turn_rate = c->Get<double> (m_name, "alignMinTurnRate", 0.05);
	m_align_threshold = c->Get<double> (m_name, "alignThreshold", 20);
	m_align_setpoint = c->Get<double> (m_name, "alignSetpoint", 0.0);
}

const AutonomousFunctions::autonomousStage
		AutonomousFunctions::DRIVE_THEN_SHOOT[DRIVE_THEN_SHOOT_LENGTH] =
		{ INIT, ADJUSTABLE_DELAY, MOVE_TO_FENDER_INIT, WAIT_FOR_POSITION, SHOOT, DONE };

const AutonomousFunctions::autonomousStage
		AutonomousFunctions::SHOOT_THEN_BRIDGE[SHOOT_THEN_BRIDGE_LENGTH] =
		{ INIT, ADJUSTABLE_DELAY, KEY_TRACK, AIM, SHOOT, DROP_WEDGE,
				MOVE_BACK_INIT, WAIT_FOR_POSITION, DONE };

const AutonomousFunctions::autonomousStage
		AutonomousFunctions::BRIDGE_THEN_SHOOT[BRIDGE_THEN_SHOOT_LENGTH] =
		{ INIT, ADJUSTABLE_DELAY, MOVE_BACK_INIT, DROP_WEDGE,
				WAIT_FOR_POSITION, RAISE_WEDGE, KEY_TRACK, AIM, SHOOT, DONE };

void AutonomousFunctions::loadQueue()
{
	while (!m_auton_sequence.empty())
		m_auton_sequence.pop();

	for (uint8_t i = 0;; ++i)
	{
		autonomousStage s = DRIVE_THEN_SHOOT[i];
//		autonomousStage s = SHOOT_THEN_BRIDGE[i];
//		autonomousStage s = BRIDGE_THEN_SHOOT[i];
		if (s == DONE)
		{
			break;
		}
		else
		{
			m_auton_sequence.push(s);
		}
	}

	m_curr_auton_stage = INIT;
}

std::string AutonomousFunctions::getAutonomousStageName(autonomousStage a)
{
	std::string str;
	switch (a)
	{
	case INIT:
		str = "Init";
		break;
	case KEY_TRACK:
		str = "Key Track";
		break;
	case AIM:
		str = "Aim";
		break;
	case SHOOT:
		str = "Shoot";
		break;
	case MOVE_BACK_INIT:
		str = "Move back init";
		break;
	case MOVE_TO_FENDER_INIT:
		str = "Move to fender init";
		break;
	case DROP_WEDGE:
		str = "Drop wedge";
		break;
	case RAISE_WEDGE:
		str = "Raise wedge";
		break;
	case WAIT_FOR_POSITION:
		str = "Wait for position";
		break;
	case DELAY_HALF_SEC:
		str = "Delay Half Second";
		break;
	case ADJUSTABLE_DELAY:
		str = "Adjustable Delay";
		break;
	case DONE:
		str = "Done";
		break;
	}
	return str;
}

void AutonomousFunctions::advanceQueue()
{
	if (!m_auton_sequence.empty())
	{
		m_curr_auton_stage = m_auton_sequence.front();
		m_auton_sequence.pop();
	}
	else
	{
		m_curr_auton_stage = DONE;
	}
	AsyncPrinter::Printf("Entering %s\r\n",
			getAutonomousStageName(m_curr_auton_stage).c_str());
}

void AutonomousFunctions::log()
{
	SmartDashboard * sdb = SmartDashboard::GetInstance();
	sdb->PutInt("Autonomous Counter", m_counter);
	sdb->PutString("Autonomous Completion State",
			ACTION::status_string[m_action->auton->completion_status]);

	std::string s;
	switch (m_action->auton->state)
	{
	case ACTION::AUTONOMOUS::AUTOALIGN:
		s = "Auto Align";
		break;
	case ACTION::AUTONOMOUS::BRIDGEBALANCE:
		s = "Bridge Balance";
		break;
	case ACTION::AUTONOMOUS::KEYTRACK:
		s = "Key Track";
		break;
	case ACTION::AUTONOMOUS::AUTON_MODE:
		s = "Auton";
		break;
	case ACTION::AUTONOMOUS::TELEOP:
		s = "Teleop";
		break;
	}
	sdb->PutString("Autonomous Mode", s.c_str());

	sdb->PutDouble("BridgePIDError", m_bridgebalance_pid.getError());
	sdb->PutDouble("BridgePID_Acc_error",
			m_bridgebalance_pid.getAccumulatedError());
	sdb->PutDouble("BridgePIDOutput", m_bridgebalance_pid.getOutput());
	sdb->PutDouble("BridgePID_P", m_bridgebalance_pid.getProportionalGain());
	sdb->PutDouble("BridgePID_I", m_bridgebalance_pid.getIntegralGain());
	sdb->PutDouble("BridgePID_D", m_bridgebalance_pid.getDerivativeGain());
	sdb->PutDouble("AutoAimPID Error", m_auto_aim_pid.getError());
	sdb->PutDouble("AutoAimPID_Acc_error", m_auto_aim_pid.getAccumulatedError());
	sdb->PutDouble("AutoAimPID_Output", m_auto_aim_pid.getOutput());
	sdb->PutDouble("AutoAimPID_P", m_auto_aim_pid.getProportionalGain());
	sdb->PutDouble("AutoAimPID_I", m_auto_aim_pid.getIntegralGain());
	sdb->PutDouble("AutoAimPID_D", m_auto_aim_pid.getDerivativeGain());
}