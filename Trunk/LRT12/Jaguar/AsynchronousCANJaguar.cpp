#include <stdio.h>
#include "../LRTRobotBase.h"
#include "../Jaguar/ProxiedCANJaguar.h"

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
			m_current(0.0),
			m_should_collect_current(false),
			m_pot_value(0.0),
			m_should_collect_pot_value(false),
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

void AsynchronousCANJaguar::ShouldCollectCurrent(bool shouldCollect)
{
	m_should_collect_current = shouldCollect;
}

void AsynchronousCANJaguar::ShouldCollectPotValue(bool shouldCollect)
{
	m_should_collect_pot_value = shouldCollect;
}

float AsynchronousCANJaguar::GetCurrent()
{
	return m_current;
}

float AsynchronousCANJaguar::GetPotValue()
{
	return m_pot_value;
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

		// if game state has changed, uncache all cached values
		if (m_last_game_state != m_game_state)
		{
			ResetCache();
		}

		// check what we can still cache
		// modes and setpoints should be automatically refreshed
		m_neutral_mode.incrementCounter();
		m_control_mode.incrementCounter();
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
		}

		if (m_forward_limit_position.hasNewValue()
				|| m_reverse_limit_position.hasNewValue())
		{
			CANJaguar::ConfigSoftPositionLimits(
					m_forward_limit_position.getValue(),
					m_reverse_limit_position.getValue());
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

		if (m_should_collect_current)
		{
			float current = CANJaguar::GetOutputCurrent();
			if (StatusOK())
				this->m_current = current;
			else
				AsynchronousPrinter::Printf("Invalid current; not storing\n");
		}

		if (m_should_collect_pot_value)
		{
			float potValue = CANJaguar::GetPosition();
			if (StatusOK())
				this->m_pot_value = potValue;
			else
				AsynchronousPrinter::Printf("Invalid pot value; not storing\n");
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
	m_setpoint.setValue(duty_cycle);
}

void AsynchronousCANJaguar::SetPosition(float position)
{
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

void AsynchronousCANJaguar::SetSpeedReference(SpeedReference reference)
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

void AsynchronousCANJaguar::ChangeControlMode(CANJaguar::ControlMode mode)
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
	m_should_disable_position_limits = false;
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
