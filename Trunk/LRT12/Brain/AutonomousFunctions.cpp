/*
 * AutonomousFunctions.cpp
 *
 *  Created on: Feb 28, 2012
 *      Author: rbtying
 */

#include "AutonomousFunctions.h"
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

		if (m_action->auton->state != m_pstate)
		{
			if (m_action->auton->state != ACTION::AUTONOMOUS::TELEOP)
			{
				m_action->auton->completion_status = ACTION::IN_PROGRESS;
				m_action->shifter->gear = ACTION::GEARBOX::LOW_GEAR;
			}
			switch (m_action->auton->state)
			{
			case ACTION::AUTONOMOUS::TELEOP:
				break;
			case ACTION::AUTONOMOUS::BRIDGEBALANCE:
				m_counter = m_timeout_bridgebalance;
				m_bridgebalance_pid->reset();
				break;
			case ACTION::AUTONOMOUS::KEYTRACK:
				m_counter = m_timeout_keytrack;
				m_prev_key_state = false;
				break;
			case ACTION::AUTONOMOUS::AUTOALIGN:
				m_counter = m_timeout_autoalign;
				break;
			}
		}

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
			// both on rate control
			m_action->drivetrain->rate.drive_control = true;
			m_action->drivetrain->rate.turn_control = true;
			m_action->drivetrain->position.drive_control = false;
			m_action->drivetrain->position.turn_control = false;
			break;
		case ACTION::AUTONOMOUS::BRIDGEBALANCE:
			bridgeBalance();
			break;
		case ACTION::AUTONOMOUS::KEYTRACK:
			keyTrack();
			break;
		case ACTION::AUTONOMOUS::AUTOALIGN:
			autoAlign();
			break;
		}
	}
}

void AutonomousFunctions::alternateBridgeBalance()
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
		m_action->auton->state = ACTION::AUTONOMOUS::TELEOP;
		m_action->auton->completion_status = ACTION::SUCCESS;
		m_action->drivetrain->rate.desiredDriveRate = 0.0;
		m_action ->drivetrain->rate.desiredTurnRate = 0.0;
	}

}

void AutonomousFunctions::bridgeBalance()
{
	m_bridgebalance_pid->setSetpoint(m_bridgebalance_setpoint);
	m_bridgebalance_pid->setInput(m_action->imu->pitch);
	m_bridgebalance_pid->update(1.0 / RobotConfig::LOOP_RATE);

	// drive on rate control, turn on rate control
	m_action->drivetrain->rate.drive_control = true;
	m_action->drivetrain->rate.turn_control = true;
	m_action->drivetrain->position.drive_control = false;
	m_action->drivetrain->position.turn_control = false;

	// set drivetrain
	m_action->drivetrain->position.desiredRelativeDrivePosition
			= m_bridgebalance_pid->getOutput();
	m_action->drivetrain->rate.desiredTurnRate = 0.0;

	if (fabs(m_bridgebalance_pid->getError()) < m_bridgebalance_threshold)
	{
		m_action->auton->state = ACTION::AUTONOMOUS::TELEOP;
		m_action->auton->completion_status = ACTION::SUCCESS;
		m_action->drivetrain->rate.drive_control = true;
		m_action->drivetrain->rate.turn_control = true;
		m_action->drivetrain->position.drive_control = false;
		m_action->drivetrain->position.turn_control = false;
		m_action->drivetrain->rate.desiredDriveRate = 0.0;
		m_action ->drivetrain->rate.desiredTurnRate = 0.0;
	}
}

void AutonomousFunctions::keyTrack()
{
	m_action->drivetrain->rate.drive_control = true;
	m_action->drivetrain->rate.turn_control = true;
	m_action->drivetrain->position.drive_control = false;
	m_action->drivetrain->position.turn_control = false;
	m_action->drivetrain->rate.desiredDriveRate = m_keytrack_forward_rate;
	m_action->drivetrain->rate.desiredTurnRate = 0.0;
	if (m_action->cam->key.higher < m_keytrack_threshold && m_prev_key_state)
	{
#warning This should change state to ACTION::AUTONOMOUS::AUTOALIGN
		m_action->auton->state = ACTION::AUTONOMOUS::TELEOP;
		m_action->auton->completion_status = ACTION::SUCCESS;
		m_action->drivetrain->rate.desiredDriveRate = 0.0;
		m_action ->drivetrain->rate.desiredTurnRate = 0.0;
	}
	m_prev_key_state = m_action->cam->key.higher > m_keytrack_threshold;
}

void AutonomousFunctions::autoAlign()
{
	double error = m_align_setpoint
			- m_action->cam->align.arbitraryOffsetFromUDP;
	m_action->drivetrain->rate.drive_control = true;
	m_action->drivetrain->rate.turn_control = true;
	m_action->drivetrain->position.drive_control = false;
	m_action->drivetrain->position.turn_control = false;

	// switch directions depending on error
	m_action->drivetrain->rate.desiredTurnRate = Util::Sign<double>(error)
			* m_align_turn_rate;

	if (fabs(error) < m_align_threshold)
	{
		m_action->auton->state = ACTION::AUTONOMOUS::TELEOP;
		m_action->auton->completion_status = ACTION::SUCCESS;
		m_action->drivetrain->rate.desiredDriveRate = 0.0;
		m_action->drivetrain->rate.desiredTurnRate = 0.0;
	}
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
	p = c->Get<double> (m_name, "bridgeP", 1.0);
	i = c->Get<double> (m_name, "bridgeI", 0.0);
	d = c->Get<double> (m_name, "bridgeD", 0.0);
	m_bridgebalance_pid->setParameters(p, i, d, 0, 1.0, false);
	m_bridgebalance_setpoint = c->Get<double> (m_name, "bridgeSetpoint", 90);
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
	case ACTION::AUTONOMOUS::TELEOP:
		s = "Teleop";
		break;
	}
}
