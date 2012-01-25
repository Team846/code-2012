#include <stdio.h>
#include "../LRTRobotBase.h"
#include "../Jaguar/AsynchronousCANJaguar.h"

#define DISABLE_SETPOINT_CACHING 0

GameState AsynchronousCANJaguar::m_game_state = DISABLED;
//ProxiedCANJaguar::JaguarList ProxiedCANJaguar::jaguars = {0};
AsynchronousCANJaguar::AsynchronousCANJaguar
		* AsynchronousCANJaguar::jaguar_list_ = NULL;

AsynchronousCANJaguar::AsynchronousCANJaguar(UINT8 channel, char* name) :
			CANJaguar(channel),
			m_task_name("JAG#" + Util::ToString<int>(channel)),
			m_print_ctor_dtor(m_task_name.c_str(), (m_task_name + "\n").c_str()),
			m_channel(channel),
			m_name(name),
			m_setpoint(0.0),
			m_should_disable_control(false),
			m_should_disable_position_limits(false),
			m_collection_flags(0),
			m_last_game_state(DISABLED),
			//    : controller(CANBusController::GetInstance())
			//    , index(jaguars.num)
			m_comm_task(m_task_name.c_str(),
					(FUNCPTR) AsynchronousCANJaguar::CommTaskWrapper),
			m_comm_semaphore(semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)),
			m_is_running(false), m_is_quitting(false)
{

	next_jaguar_ = jaguar_list_;
	jaguar_list_ = this;

	if (m_name == NULL)
		m_name = "?";
	m_comm_task.Start((UINT32) this);

	m_enable_control.disableCaching();

	printf("Created Jaguar %2d: %s\n", channel, m_name);

}

AsynchronousCANJaguar::~AsynchronousCANJaguar()
{
	//Before we delete the jaguar object,
	//the Jaguar reader task should be killed
	// and the main loop that accesses jags should be killed.
	// currently the main loop is killed in the dtor of LRTRobot11.
	//   JaguarReader::GetInstance().StopTask(); //kill the jag reader that accesses this object.
	StopBackgroundTask();
	int error = semDelete(m_comm_semaphore);
	if (error)
		printf("SemDelete Error=%d\n", error);
}
void AsynchronousCANJaguar::StopBackgroundTask()
{
	if (m_is_running)
	{
		INT32 task_id = m_comm_task.GetID(); //for info only. no safety check.
		m_comm_task.Stop();
		printf("Task 0x%x killed for CANid=%d:%s\n", task_id, m_channel, m_name);
	}
}

int AsynchronousCANJaguar::CommTaskWrapper(UINT32 proxiedCANJaguarPointer)
{
	AsynchronousCANJaguar* jaguar =
			(AsynchronousCANJaguar*) proxiedCANJaguarPointer;
	if (jaguar->m_channel != 0) // ignore jags on channel #0 -dg
	{
		jaguar->m_is_running = true;
		jaguar->CommTask();
	}
	jaguar->m_is_running = false;
	printf("Ending task for Jaguar %s %d\n", jaguar->m_name, jaguar->m_channel);
	return 0; // return no error
}

void AsynchronousCANJaguar::CommTask()
{
	while (!m_is_quitting)
	{
		semTake(m_comm_semaphore, WAIT_FOREVER);
		if (m_is_quitting)
			break;

		// set data
		// if game state has changed, uncache all cached values
		if (m_last_game_state != m_game_state)
		{
			ResetCache();
			// TODO: FINISH UP HERE, reset appropriate flags
		}

		// check what we can still cache
		// modes and setpoints should be automatically refreshed
		m_neutral_mode.incrementCounter();
		// m_control_mode.incrementCounter(); do not resend control mode periodically
		m_setpoint.incrementCounter();

		// if any of the modes or setpoints have changed
		// resend all of them
		if (m_neutral_mode.hasNewValue() || m_control_mode.hasNewValue()
				|| m_setpoint.hasNewValue())
		{
			m_neutral_mode.uncache();
			m_control_mode.uncache();
			m_setpoint.uncache();
		}

#if DISABLE_SETPOINT_CACHING
		ResetCache();
#endif

		if (m_control_mode.hasNewValue())
		{
			CANJaguar::ChangeControlMode(m_control_mode.getValue());

			m_setpoint.uncache();
			m_neutral_mode.uncache();
			m_pid_p.uncache();
			m_pid_i.uncache();
			m_pid_d.uncache();
			m_position_reference.uncache();
			m_speed_reference.uncache();
			m_enable_control.uncache();
			m_voltage_ramp_rate.uncache();
			m_fault_time.uncache();
			m_encoder_codes_per_rev.uncache();
			m_potentiometer_turns.uncache();
			m_max_output_voltage.uncache();
		}

		if (m_position_reference.hasNewValue())
		{
			CANJaguar::SetPositionReference(m_position_reference.getValue());
		}

		if (m_speed_reference.hasNewValue())
		{
			CANJaguar::SetSpeedReference(m_speed_reference.getValue());
		}

		if (m_pid_p.hasNewValue() || m_pid_i.hasNewValue()
				|| m_pid_d.hasNewValue())
		{
			CANJaguar::SetPID(m_pid_p.getValue(), m_pid_i.getValue(),
					m_pid_d.getValue());
		}

		if (m_voltage_ramp_rate.hasNewValue())
		{
			CANJaguar::SetVoltageRampRate(m_voltage_ramp_rate.getValue());
		}

		if (m_max_output_voltage.hasNewValue())
		{
			CANJaguar::ConfigMaxOutputVoltage(m_max_output_voltage.getValue());
		}

		if (m_fault_time.hasNewValue())
		{
			CANJaguar::ConfigFaultTime(m_fault_time.getValue());
		}

		if (m_expiration.hasNewValue())
		{
			CANJaguar::SetExpiration(m_expiration.getValue());
		}

		if (m_encoder_codes_per_rev.hasNewValue())
		{
			CANJaguar::ConfigEncoderCodesPerRev(
					m_encoder_codes_per_rev.getValue());
		}

		if (m_potentiometer_turns.hasNewValue())
		{
			CANJaguar::ConfigPotentiometerTurns(
					m_potentiometer_turns.getValue());
		}

		if (m_should_disable_position_limits)
		{
			CANJaguar::DisableSoftPositionLimits();
			m_should_disable_position_limits = false;
		}

		if (m_forward_limit_position.hasNewValue()
				|| m_reverse_limit_position.hasNewValue())
		{
			CANJaguar::ConfigSoftPositionLimits(
					m_forward_limit_position.getValue(),
					m_reverse_limit_position.getValue());
		}

		if (m_enable_control.hasNewValue())
		{
			CANJaguar::EnableControl(m_enable_control.getValue());
		}

		if (m_should_disable_control)
		{
			CANJaguar::DisableControl();
			m_should_disable_control = false;
		}

		//change the mode, then do the set point.
		if (m_neutral_mode.hasNewValue())
		{
			CANJaguar::ConfigNeutralMode(m_neutral_mode.getValue());
		}

		if (m_setpoint.hasNewValue())
		{
			CANJaguar::Set(m_setpoint.getValue());
		}

		// collect data	
		if (m_collection_flags & OUTVOLT)
		{
			float v = CANJaguar::GetOutputVoltage();
			if (StatusOK())
				m_output_voltage = v;
			else
				AsynchronousPrinter::Printf(
						"Invalid output voltage; not storing\n");
		}

		if (m_collection_flags & OUTCURR)
		{
			float current = CANJaguar::GetOutputCurrent();
			if (StatusOK())
				m_output_current = current;
			else
				AsynchronousPrinter::Printf("Invalid current; not storing\n");
		}

		if (m_collection_flags & POS)
		{
			float pos = CANJaguar::GetPosition();
			if (StatusOK())
				m_position = pos;
			else
				AsynchronousPrinter::Printf(
						"Invalid position value; not storing\n");
		}

		if (m_collection_flags & PID)
		{
			float p = CANJaguar::GetP();
			if (StatusOK())
				m_p = p;
			else
				AsynchronousPrinter::Printf(
						"Invalid proportional gain; not storing\n");
			float i = CANJaguar::GetI();
			if (StatusOK())
				m_i = i;
			else
				AsynchronousPrinter::Printf(
						"Invalid integral gain; not storing\n");
			float d = CANJaguar::GetD();
			if (StatusOK())
				m_d = d;
			else
				AsynchronousPrinter::Printf(
						"Invalid derivative gain; not storing\n");
		}

		if (m_collection_flags & SPEEDREF)
		{
			SpeedReference s = CANJaguar::GetSpeedReference();
			if (StatusOK())
				m_speed_ref = s;
			else
				AsynchronousPrinter::Printf(
						"Invalid speed reference; not storing\n");
		}

		if (m_collection_flags & POSREF)
		{
			PositionReference p = CANJaguar::GetPositionReference();
			if (StatusOK())
				m_position_ref = p;
			else
				AsynchronousPrinter::Printf(
						"Invalid position reference; not storing");
		}

		if (m_collection_flags & CTRLMODE)
		{
			ControlMode c = CANJaguar::GetControlMode();
			if (StatusOK())
				m_ctrl_mode = c;
			else
				AsynchronousPrinter::Printf(
						"Invalid control mode; not storing\n");
		}

		if (m_collection_flags & BUSVOLT)
		{
			float v = CANJaguar::GetBusVoltage();
			if (StatusOK())
				m_bus_voltage = v;
			else
				AsynchronousPrinter::Printf(
						"Invalid bus voltage; not storing\n");
		}

		if (m_collection_flags & TEMP)
		{
			float t = CANJaguar::GetTemperature();
			if (StatusOK())
				m_temperature = t;
			else
				AsynchronousPrinter::Printf(
						"Invalid temperature; not storing\n");
		}

		if (m_collection_flags & SPEED)
		{
			float s = CANJaguar::GetSpeed();
			if (StatusOK())
				m_speed = s;
			else
				AsynchronousPrinter::Printf("Invalid speed; not storing\n");
		}

		if (m_collection_flags & FWDLIMOK)
		{
			bool b = CANJaguar::GetForwardLimitOK();
			if (StatusOK())
				m_fwd_limit_ok = b;
			else
				AsynchronousPrinter::Printf(
						"Invalid forward limit status, not storing\n");
		}

		if (m_collection_flags & REVLIMOK)
		{
			bool b = CANJaguar::GetReverseLimitOK();
			if (StatusOK())
				m_rev_limit_ok = b;
			else
				AsynchronousPrinter::Printf(
						"Invalid reverse limit status, not storing\n");
		}

		if (m_collection_flags & PWRCYCLE)
		{
			bool b = CANJaguar::GetPowerCycled();
			if (StatusOK())
				m_pwr_cyc = b;
			else
				AsynchronousPrinter::Printf(
						"Invalid power cycle status, not storing\n");
		}

		if (m_collection_flags & EXPIRE)
		{
			float e = CANJaguar::GetExpiration();
			if (StatusOK())
				m_expire = e;
			else
				AsynchronousPrinter::Printf(
						"Invalid expiration time, not storing\n");
		}

		m_last_game_state = m_game_state;
		//        AsynchronousPrinter::Printf("%d\n", channel);
	}
}

void AsynchronousCANJaguar::BeginComm()
{
	semGive(m_comm_semaphore);
}

//Set() is ambiguous, since it doesn't include the mode.
//We've replaced them with specific command.  They are still in progress.  TODO -dg
void AsynchronousCANJaguar::SetDutyCycle(float duty_cycle)
{
	m_control_mode.setValue(kPercentVbus);
	m_setpoint.setValue(duty_cycle);
}

void AsynchronousCANJaguar::SetPosition(float position)
{
	m_control_mode.setValue(kPosition);
	m_setpoint.setValue(position);
}

void AsynchronousCANJaguar::Set(float setpoint, UINT8 syncGroup)
{
	printf("ERROR: Calling Set() in ProxiedCANJaguar: %s\n;", m_name);
	m_setpoint.setValue(setpoint);
}

void AsynchronousCANJaguar::SetPositionReference(
		CANJaguar::PositionReference reference)
{
	m_position_reference.setValue(reference);
}

void AsynchronousCANJaguar::SetSpeedReference(CANJaguar::SpeedReference reference)
{
	m_speed_reference.setValue(reference);
}

void AsynchronousCANJaguar::SetPID(double p, double i, double d)
{
	m_pid_p.setValue(p);
	m_pid_i.setValue(i);
	m_pid_d.setValue(d);
}

void AsynchronousCANJaguar::ConfigNeutralMode(NeutralMode mode)
{
	m_neutral_mode.setValue(mode);
}

void AsynchronousCANJaguar::ChangeControlMode(ControlMode mode)
{
	m_control_mode.setValue(mode);
}

void AsynchronousCANJaguar::EnableControl(double encoderInitialPosition)
{
	m_enable_control.setValue(encoderInitialPosition);
}

void AsynchronousCANJaguar::DisableControl()
{
	m_should_disable_control = true;
}

void AsynchronousCANJaguar::SetGameState(GameState state)
{
	m_game_state = state;
}

void AsynchronousCANJaguar::SetVoltageRampRate(double rampRate)
{
	m_voltage_ramp_rate.setValue(rampRate);
}

void AsynchronousCANJaguar::ConfigEncoderCodesPerRev(UINT16 codesPerRev)
{
	m_encoder_codes_per_rev.setValue(codesPerRev);
}

void AsynchronousCANJaguar::ConfigPotentiometerTurns(UINT16 turns)
{
	m_potentiometer_turns.setValue(turns);
}

void AsynchronousCANJaguar::ConfigSoftPositionLimits(
		double forwardLimitPosition, double reverseLimitPosition)
{
	m_forward_limit_position.setValue(forwardLimitPosition);
	m_reverse_limit_position.setValue(reverseLimitPosition);
}

void AsynchronousCANJaguar::DisableSoftPositionLimits()
{
	m_should_disable_position_limits = true;
}

void AsynchronousCANJaguar::ConfigMaxOutputVoltage(double voltage)
{
	m_max_output_voltage.setValue(voltage);
}

void AsynchronousCANJaguar::ConfigFaultTime(float faultTime)
{
	m_fault_time.setValue(faultTime);
}

void AsynchronousCANJaguar::SetExpiration(float timeout)
{
	m_expiration.setValue(timeout);
}

void AsynchronousCANJaguar::ResetCache()
{
	m_setpoint.uncache();
	m_neutral_mode.uncache();
	m_control_mode.uncache();
}

bool AsynchronousCANJaguar::StatusOK()
{
	return GetFaults() == 0;
}

void AsynchronousCANJaguar::removeCollectionFlags(uint32_t flags)
{
	m_collection_flags &= ~(flags);
}

void AsynchronousCANJaguar::addCollectionFlags(uint32_t flags)
{
	m_collection_flags |= flags;
}

void AsynchronousCANJaguar::setCollectionFlags(uint32_t flags)
{
	m_collection_flags = flags;
}
