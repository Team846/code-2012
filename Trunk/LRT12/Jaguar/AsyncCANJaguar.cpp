#include <stdio.h>
#include "../LRTRobotBase.h"
#include "../Jaguar/AsyncCANJaguar.h"

#define DISABLE_SETPOINT_CACHING 0

GameState AsyncCANJaguar::m_game_state = DISABLED;
//ProxiedCANJaguar::JaguarList ProxiedCANJaguar::jaguars = {0};
AsyncCANJaguar::AsyncCANJaguar * AsyncCANJaguar::jaguar_list_ = NULL;

void AsyncCANJaguar::println(const char * str)
{
	AsyncPrinter::Printf("%s: %s", m_name, str);
}

AsyncCANJaguar::AsyncCANJaguar(UINT8 channel, char* name) :
			CANJaguar(channel),
			Loggable(),
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
					(FUNCPTR) AsyncCANJaguar::CommTaskWrapper),
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

AsyncCANJaguar::~AsyncCANJaguar()
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
void AsyncCANJaguar::StopBackgroundTask()
{
	if (m_is_running)
	{
		INT32 task_id = m_comm_task.GetID(); //for info only. no safety check.
		m_comm_task.Stop();
		printf("Task 0x%x killed for CANid=%d:%s\n", task_id, m_channel, m_name);
	}
}

int AsyncCANJaguar::CommTaskWrapper(UINT32 proxiedCANJaguarPointer)
{
	AsyncCANJaguar* jaguar = (AsyncCANJaguar*) proxiedCANJaguarPointer;
	if (jaguar->m_channel != 0) // ignore jags on channel #0 -dg
	{
		jaguar->m_is_running = true;
		jaguar->CommTask();
	}
	jaguar->m_is_running = false;
	printf("Ending task for Jaguar %s %d\n", jaguar->m_name, jaguar->m_channel);
	return 0; // return no error
}

void AsyncCANJaguar::CommTask()
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
			ResetCache();
			CANJaguar::ChangeControlMode(m_control_mode.getValue());
			// note that m_control_mode should be uncached as of this point
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
				println("Invalid output voltage; not storing\n");
		}

		if (m_collection_flags & OUTCURR)
		{
			float current = CANJaguar::GetOutputCurrent();
			if (StatusOK())
				m_output_current = current;
			else
				println("Invalid current; not storing\n");
		}

		if (m_collection_flags & POS)
		{
			float pos = CANJaguar::GetPosition();
			if (StatusOK())
				m_position = pos;
			else
				println("Invalid position value; not storing\n");
		}

		if (m_collection_flags & PID)
		{
			float p = CANJaguar::GetP();
			if (StatusOK())
				m_p = p;
			else
				println("Invalid proportional gain; not storing\n");
			float i = CANJaguar::GetI();
			if (StatusOK())
				m_i = i;
			else
				println("Invalid integral gain; not storing\n");
			float d = CANJaguar::GetD();
			if (StatusOK())
				m_d = d;
			else
				println("Invalid derivative gain; not storing\n");
		}

		if (m_collection_flags & SPEEDREF)
		{
			SpeedReference s = CANJaguar::GetSpeedReference();
			if (StatusOK())
				m_speed_ref = s;
			else
				println("Invalid speed reference; not storing\n");
		}

		if (m_collection_flags & POSREF)
		{
			PositionReference p = CANJaguar::GetPositionReference();
			if (StatusOK())
				m_position_ref = p;
			else
				println("Invalid position reference; not storing");
		}

		if (m_collection_flags & CTRLMODE)
		{
			ControlMode c = CANJaguar::GetControlMode();
			if (StatusOK())
				m_ctrl_mode = c;
			else
				println("Invalid control mode; not storing\n");
		}

		if (m_collection_flags & BUSVOLT)
		{
			float v = CANJaguar::GetBusVoltage();
			if (StatusOK())
				m_bus_voltage = v;
			else
				println("Invalid bus voltage; not storing\n");
		}

		if (m_collection_flags & TEMP)
		{
			float t = CANJaguar::GetTemperature();
			if (StatusOK())
				m_temperature = t;
			else
				println("Invalid temperature; not storing\n");
		}

		if (m_collection_flags & SPEED)
		{
			float s = CANJaguar::GetSpeed();
			if (StatusOK())
				m_speed = s;
			else
				println("Invalid speed; not storing\n");
		}

		if (m_collection_flags & FWDLIMOK)
		{
			bool b = CANJaguar::GetForwardLimitOK();
			if (StatusOK())
				m_fwd_limit_ok = b;
			else
				println("Invalid forward limit status, not storing\n");
		}

		if (m_collection_flags & REVLIMOK)
		{
			bool b = CANJaguar::GetReverseLimitOK();
			if (StatusOK())
				m_rev_limit_ok = b;
			else
				println("Invalid reverse limit status, not storing\n");
		}

		if (m_collection_flags & PWRCYCLE)
		{
			bool b = CANJaguar::GetPowerCycled();
			if (StatusOK())
				m_pwr_cyc = b;
			else
				println("Invalid power cycle status, not storing\n");
		}

		if (m_collection_flags & EXPIRE)
		{
			float e = CANJaguar::GetExpiration();
			if (StatusOK())
				m_expire = e;
			else
				println("Invalid expiration time, not storing\n");
		}

		m_last_game_state = m_game_state;
		//        AsynchronousPrinter::Printf("%d\n", channel);
	}
}

void AsyncCANJaguar::BeginComm()
{
	semGive(m_comm_semaphore);
}

//Set() is ambiguous, since it doesn't include the mode.
//We've replaced them with specific command.  They are still in progress.  TODO -dg
void AsyncCANJaguar::SetDutyCycle(float duty_cycle)
{
	m_control_mode.setValue(kPercentVbus);
	m_setpoint.setValue(duty_cycle);
}

void AsyncCANJaguar::SetPosition(float position)
{
	m_control_mode.setValue(kPosition);
	m_setpoint.setValue(position);
}

void AsyncCANJaguar::SetVelocity(float velocity)
{
	m_control_mode.setValue(kSpeed);
	m_setpoint.setValue(velocity);
}

void AsyncCANJaguar::Set(float setpoint, UINT8 syncGroup)
{
	printf("ERROR: Calling Set() in ProxiedCANJaguar: %s\n;", m_name);
	m_setpoint.setValue(setpoint);
}

void AsyncCANJaguar::SetPositionReference(
		CANJaguar::PositionReference reference)
{
	m_position_reference.setValue(reference);
}

void AsyncCANJaguar::SetSpeedReference(CANJaguar::SpeedReference reference)
{
	m_speed_reference.setValue(reference);
}

void AsyncCANJaguar::SetPID(double p, double i, double d)
{
	m_pid_p.setValue(p);
	m_pid_i.setValue(i);
	m_pid_d.setValue(d);
}

void AsyncCANJaguar::ConfigNeutralMode(NeutralMode mode)
{
	m_neutral_mode.setValue(mode);
}

void AsyncCANJaguar::ChangeControlMode(ControlMode mode)
{
	m_control_mode.setValue(mode);
}

void AsyncCANJaguar::EnableControl(double encoderInitialPosition)
{
	m_enable_control.setValue(encoderInitialPosition);
}

void AsyncCANJaguar::DisableControl()
{
	m_should_disable_control = true;
}

void AsyncCANJaguar::SetGameState(GameState state)
{
	m_game_state = state;
}

void AsyncCANJaguar::SetVoltageRampRate(double rampRate)
{
	m_voltage_ramp_rate.setValue(rampRate);
}

void AsyncCANJaguar::ConfigEncoderCodesPerRev(UINT16 codesPerRev)
{
	m_encoder_codes_per_rev.setValue(codesPerRev);
}

void AsyncCANJaguar::ConfigPotentiometerTurns(UINT16 turns)
{
	m_potentiometer_turns.setValue(turns);
}

void AsyncCANJaguar::ConfigSoftPositionLimits(double forwardLimitPosition,
		double reverseLimitPosition)
{
	m_forward_limit_position.setValue(forwardLimitPosition);
	m_reverse_limit_position.setValue(reverseLimitPosition);
}

void AsyncCANJaguar::DisableSoftPositionLimits()
{
	m_should_disable_position_limits = true;
}

void AsyncCANJaguar::ConfigMaxOutputVoltage(double voltage)
{
	m_max_output_voltage.setValue(voltage);
}

void AsyncCANJaguar::ConfigFaultTime(float faultTime)
{
	m_fault_time.setValue(faultTime);
}

void AsyncCANJaguar::SetExpiration(float timeout)
{
	m_expiration.setValue(timeout);
}

void AsyncCANJaguar::ResetCache()
{
	m_control_mode.uncache();
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

bool AsyncCANJaguar::StatusOK()
{
	return GetFaults() == 0;
}

void AsyncCANJaguar::removeCollectionFlags(uint32_t flags)
{
	m_collection_flags &= ~(flags);
}

void AsyncCANJaguar::addCollectionFlags(uint32_t flags)
{
	m_collection_flags |= flags;
}

void AsyncCANJaguar::setCollectionFlags(uint32_t flags)
{
	m_collection_flags = flags;
}

void AsyncCANJaguar::log()
{
	SmartDashboard *sdb = SmartDashboard::GetInstance();
	std::string prefix(m_name);
	prefix += ": ";

	std::string cv("None");
	if (m_collection_flags & SPEEDREF)
	{
		cv += "Speed Reference, ";
		std::string out;
		switch (GetSpeedReference())
		{
		case kSpeedRef_Encoder:
			out = "Encoder";
			break;
		case kSpeedRef_InvEncoder:
			out = "Indexing Encoder";
			break;
		case kSpeedRef_QuadEncoder:
			out = "Quadrature Encoder";
			break;
		case kSpeedRef_None:
			out = "None";
			break;
		}
		sdb->PutString(prefix + "Speed Reference", out);
	}
	if (m_collection_flags & POSREF)
	{
		cv += "Position Reference, ";
		std::string out;
		switch (GetPositionReference())
		{
		case kPosRef_Potentiometer:
			out = "Potentiometer";
			break;
		case kPosRef_QuadEncoder:
			out = "Quadrature Encoder";
			break;
		case kPosRef_None:
			out = "None";
			break;
		}
		sdb->PutString(prefix + "Position Reference", out);
	}
	if (m_collection_flags & PID)
	{
		cv += "PID, ";
		sdb->PutDouble((prefix + "Proportional Gain").c_str(), GetP());
		sdb->PutDouble((prefix + "Integral Gain").c_str(), GetI());
		sdb->PutDouble((prefix + "Derivative Gain").c_str(), GetD());
	}
	if (m_collection_flags & CTRLMODE)
	{
		cv += "Control Mode, ";
		std::string out;
		switch (GetControlMode())
		{
		case kPercentVbus:
			out = "Duty Cycle";
			break;
		case kVoltage:
			out = "Voltage";
			break;
		case kSpeed:
			out = "Speed";
			break;
		case kPosition:
			out = "Position";
			break;
		case kCurrent:
			out = "Current";
			break;
		}
		sdb->PutString(prefix + "Control Mode (read)", out);
	}
	if (m_collection_flags & BUSVOLT)
	{
		cv += "Bus Voltage, ";
		sdb->PutDouble((prefix + "Bus Voltage").c_str(), GetBusVoltage());
	}
	if (m_collection_flags & OUTVOLT)
	{
		cv += "Output Voltage, ";
		sdb->PutDouble((prefix + "Output Voltage").c_str(), GetOutputVoltage());
	}
	if (m_collection_flags & OUTCURR)
	{
		cv += "Output Current, ";
		sdb->PutDouble((prefix + "Output Current").c_str(), GetOutputCurrent());
	}
	if (m_collection_flags & TEMP)
	{
		cv += "Temperature, ";
		sdb->PutDouble((prefix + "Temperature").c_str(), GetTemperature());
	}
	if (m_collection_flags & FWDLIMOK)
	{
		cv += "Forward Soft Limit OK, ";
		sdb->PutBoolean((prefix + "Forward Limit OK").c_str(),
				GetForwardLimitOK());
	}
	if (m_collection_flags & REVLIMOK)
	{
		cv += "Reverse Soft Limit OK, ";
		sdb->PutBoolean((prefix + "Reverse Limit OK").c_str(),
				GetReverseLimitOK());
	}
	if (m_collection_flags & PWRCYCLE)
	{
		cv += "Power Cycled, ";
		sdb->PutBoolean((prefix + "Power Cycled").c_str(), GetPowerCycled());
	}
	if (m_collection_flags & EXPIRE)
	{
		cv += "Expiration";
		sdb->PutDouble((prefix + "Expiration").c_str(), GetExpiration());
	}

	std::string out;
	switch (m_control_mode.peek())
	{
	case kPercentVbus:
		out = "Duty Cycle";
		break;
	case kVoltage:
		out = "Voltage";
		break;
	case kSpeed:
		out = "Speed";
		break;
	case kPosition:
		out = "Position";
		break;
	case kCurrent:
		out = "Current";
		break;
	}

	sdb->PutString(prefix + "Control Mode", out);
	sdb->PutDouble((prefix + "Setpoint").c_str(), m_setpoint.peek());
	sdb->PutString(prefix + "Enabled Collection Values", cv);
}