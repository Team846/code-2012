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
#include "../Util/PID.h"

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
	m_bridgebalance_pid = new PID();
	Configure();
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

	delete m_bridgebalance_pid;
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
				m_bridgebalance_pid->reset();
				break;
			case ACTION::AUTONOMOUS::KEYTRACK:
				m_counter = m_timeout_keytrack;
				break;
			case ACTION::AUTONOMOUS::AUTOALIGN:
				m_counter = m_timeout_autoalign;
				break;
			case ACTION::AUTONOMOUS::AUTON_MODE:
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
				m_action->auton->state = ACTION::AUTONOMOUS::TELEOP;
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
				m_action->auton->state = ACTION::AUTONOMOUS::TELEOP;
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
	if (++e % 50 == 0)
		AsyncPrinter::Printf("Balancing\n");

	//	m_bridgebalance_pid->setSetpoint(m_bridgebalance_setpoint);
	m_bridgebalance_pid->setSetpoint(0);
	m_bridgebalance_pid->setInput(m_action->imu->pitch);
	m_bridgebalance_pid->update(1.0 / RobotConfig::LOOP_RATE);

	// drive on rate control, turn on rate control
	m_action->drivetrain->rate.drive_control = true;
	m_action->drivetrain->rate.turn_control = true;
	m_action->drivetrain->position.drive_control = false;
	m_action->drivetrain->position.turn_control = false;

	// set drivetrain and switch sign
	//	m_action->drivetrain->position.desiredRelativeDrivePosition
	//			= -m_bridgebalance_pid->getOutput();
	m_action->drivetrain->rate.desiredDriveRate = Util::Clamp<double>(
			-m_bridgebalance_pid->getOutput(), -0.1, 0.1);

	//	if (++e % 25 == 0)
	//		AsyncPrinter::Printf("Bridge Out %.4f\n",
	//				m_bridgebalance_pid->getOutput());
	//	m_action->drivetrain->rate.desiredDriveRate
	//			= m_bridgebalance_pid->getOutput();
	m_action->drivetrain->rate.desiredTurnRate = 0.0;

	if (fabs(m_bridgebalance_pid->getError()) < m_bridgebalance_threshold)
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

	//	AsyncPrinter::Printf("%d %d %d %f\n", m_hit_key_flag, state,
	//			m_action->cam->key.higher, m_keytrack_threshold);
	if (m_hit_key_flag)
	{
		if (!state)
		{
			m_hit_key_flag = false;
			m_action->drivetrain->rate.desiredDriveRate = 0.0;
			m_action ->drivetrain->rate.desiredTurnRate = 0.0;
			return true;

		}
	}
	return false;
	//	AsyncPrinter::Printf("%d %d\n", state, m_hit_key_flag);
}

bool AutonomousFunctions::autoAlign()
{
	if (m_action->cam->align.status != ACTION::CAMERA::NO_TARGET)
	{
		double error = m_align_setpoint
				- m_action->cam->align.arbitraryOffsetFromUDP;
		m_action->drivetrain->rate.drive_control = true;
		m_action->drivetrain->rate.turn_control = true;
		m_action->drivetrain->position.drive_control = false;
		m_action->drivetrain->position.turn_control = false;

		// switch directions depending on error
		m_action->drivetrain->rate.desiredTurnRate = Util::Sign<double>(error)
				* fabs(error * (1.0 / 127) * m_align_turn_rate);
		m_action->drivetrain->rate.desiredDriveRate = 0.0;

		if (fabs(error) < m_align_threshold )
		{
			AsyncPrinter::Printf("Aiming done\n");
			//			m_action->auton->state = ACTION::AUTONOMOUS::TELEOP;
			
			return true;
			//			m_action->drivetrain->rate.desiredDriveRate = 0.0;
			//			m_action->drivetrain->rate.desiredTurnRate = 0.0;
		}
	}
	return false;
}

#define BALLS_TO_SHOOT 2
bool AutonomousFunctions::autonomousMode()
{
	switch (m_curr_auton_stage)
	{
	case INIT:
#warning Set speed correctly and check trajectory 
		m_action->launcher->desiredSpeed = ACTION::LAUNCHER::MEDIUM;
		m_action->launcher->atSpeed = false;
		m_action->launcher->topTrajectory = false;
		m_curr_auton_stage = KEY_TRACK;
		break;
	case KEY_TRACK:
		if (keyTrack())
		{
			m_curr_auton_stage = AIM;
		}
		break;
	case AIM:
		if (autoAlign())
		{
			m_curr_auton_stage = SHOOT;
			m_shot_counter = 0;
		}
		break;
	case SPIN_UP:
		if (m_action->launcher->atSpeed)
		{
			m_launcher_was_at_speed = true;
			m_curr_auton_stage = SHOOT;
		}
		break;
	case SHOOT:
		m_action->wedge->state = ACTION::WEDGE::PRESET_BOTTOM;
		if (m_shot_counter >= BALLS_TO_SHOOT)
		{
			m_action->ballfeed->attemptToLoadRound = true;
			if ((!m_action->launcher->atSpeed) && m_launcher_was_at_speed) //indicates ball launched
				m_shot_counter++;
			m_launcher_was_at_speed = m_action->launcher->atSpeed;
		}
		else 
		{
			m_curr_auton_stage = MOVE_BACK_INIT;
		}
		break;
	case MOVE_BACK_INIT:
		
		m_action->drivetrain->position.absolute = false;
		m_action->drivetrain->position.drive_control = true;
		m_action->drivetrain->position.turn_control = true;
#warning Verify amount to drive back
		m_action->drivetrain->position.desiredRelativeDrivePosition = -12; //TODO Check me
//		m_action->drivetrain->position.desiredRelativeDrivePosition = -120; //TODO Check me
		m_action->drivetrain->position.desiredRelativeTurnPosition = 0;
		m_curr_auton_stage = MOVE_BACK;
		break;
	case MOVE_BACK:
		m_action->wedge->state = ACTION::WEDGE::PRESET_BOTTOM;
		if (m_action->drivetrain->previousDriveOperationComplete)
			m_curr_auton_stage = DONE;
		break;
	case DONE:
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
	m_bridgebalance_pid->setParameters(p, i, d, 0, 0.87, false);
	m_bridgebalance_setpoint = c->Get<double> (m_name, "bridgeSetpoint", 00);
	m_bridgebalance_threshold = c->Get<double> (m_name, "bridgeThreshold", 1);
	m_bridgebalance_angular_rate_threshold = c->Get<double> (m_name,
			"bridgeAngularRateThreshold", 30);

	m_keytrack_forward_rate = c->Get<double> (m_name, "keyForwardRate", 0.1);
	m_keytrack_threshold = c->Get<double> (m_name, "keyThreshold", 127);

	m_align_turn_rate = c->Get<double> (m_name, "alignTurnRate", 0.15);
	m_align_threshold = c->Get<double> (m_name, "alignThreshold", 20);
	m_align_setpoint = c->Get<double> (m_name, "alignSetpoint", 0.0);
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

	sdb->PutDouble("BridgePIDError", m_bridgebalance_pid->getError());
	sdb->PutDouble("BridgePID_Acc_error",
			m_bridgebalance_pid->getAccumulatedError());
	sdb->PutDouble("BridgePIDOutput", m_bridgebalance_pid->getOutput());
	sdb->PutDouble("BridgePID_P", m_bridgebalance_pid->getProportionalGain());
	sdb->PutDouble("BridgePID_I", m_bridgebalance_pid->getDerivativeGain());
	sdb->PutDouble("BridgePID_D", m_bridgebalance_pid->getIntegralGain());
}
