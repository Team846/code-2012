#ifndef PROXIED_CAN_JAGUAR_H_
#define PROXIED_CAN_JAGUAR_H_

#include "taskLib.h"

#include "WPILib/CANJaguar.h"

#include "../Util/Util.h"
#include "../Util/PrintInConstructor.h"
#include "../Util/CachedValue.h"

/*!
 * @brief Asynchronous implementation of a CAN driver for the Jaguar
 * @brief Reimplements the large majority of useful operations on the Jaguar
 * @brief in a threaded fashion.
 * 
 * @author Robert Ying
 * @author Brian Axelrod
 * @author Karthik Viswanathan
 * @author David Giandomenico
 */
class AsynchronousCANJaguar: public CANJaguar
{
private:
	string m_task_name;
	PrintInConstructor m_print_ctor_dtor;
	int m_channel;
	char* m_name;

	CachedValue<volatile float> m_setpoint;
	CachedValue<volatile NeutralMode> m_neutral_mode;
	CachedValue<volatile ControlMode> m_control_mode;
	CachedValue<volatile PositionReference> m_position_reference;
	CachedValue<volatile SpeedReference> m_speed_reference;

	CachedValue<volatile double> m_pid_p;
	CachedValue<volatile double> m_pid_i;
	CachedValue<volatile double> m_pid_d;

	CachedValue<volatile double> m_enable_control;
	volatile bool m_should_disable_control;

	CachedValue<volatile double> m_voltage_ramp_rate;
	CachedValue<volatile double> m_max_output_voltage;
	CachedValue<volatile float> m_fault_time;
	CachedValue<volatile float> m_expiration;
	CachedValue<volatile UINT16> m_encoder_codes_per_rev;
	CachedValue<volatile UINT16> m_potentiometer_turns;

	CachedValue<volatile double> m_forward_limit_position;
	CachedValue<volatile double> m_reverse_limit_position;
	volatile bool m_should_disable_position_limits;

	volatile float m_current;
	volatile bool m_should_collect_current;

	volatile float m_pot_value;
	volatile bool m_should_collect_pot_value;

	static GameState m_game_state;
	GameState m_last_game_state;

	int m_index;
	Task m_comm_task;

	semaphore* m_comm_semaphore;
	bool m_is_running; //implementation in progress - controlled termination of task -dg
	bool m_is_quitting; // ditto

public:
	/*!
	 * @brief Constructs a new Jaguar on the CAN network
	 * @param channel Channel that the Jaguar is assigned to
	 * @param name The name of the Jaguar
	 */
	AsynchronousCANJaguar(UINT8 channel, char* name);

	/*!
	 * @briefDestroys the Jaguar object and stops the thread
	 */
	~AsynchronousCANJaguar();

	/*!
	 * @brief Stops the background thread
	 */
	void StopBackgroundTask();

	/*!
	 * @brief LinkedList of all of the constructed CAN Jaguars
	 */
	static AsynchronousCANJaguar* jaguar_list_;

	/*!
	 * @brief Pointer to next Jaguar
	 */
	AsynchronousCANJaguar* next_jaguar_;

	/*!
	 * @brief Set the duty cycle of the Jaguar
	 * @param duty_cycle
	 */
	void SetDutyCycle(float duty_cycle);

	/*!
	 * @brief Set the output position of the Jaguar
	 * @param position
	 */
	void SetPosition(float position);

private:
	//don't let external objects control the ESC with the ambiguous Set() cmd
	void Set(float setpoint, UINT8 syncGroup = 0);

public:
	/*!
	 * @brief Set the PID parameters
	 * @param p proportional gain
	 * @param i integral gain
	 * @param d derivative gain
	 */
	void SetPID(double p, double i, double d);

	/*!
	 * @brief Set the new neutral mode
	 * @param mode the new mode
	 */
	void ConfigNeutralMode(CANJaguar::NeutralMode mode);

	/*!
	 * @brief Set the new speed reference
	 * @param reference the new reference
	 */
	void SetSpeedReference(SpeedReference reference);

	/*!
	 * @brief Set the new control mode
	 * @param mode the new mode
	 */
	void ChangeControlMode(CANJaguar::ControlMode mode);

	/*!
	 * @brief Enables control
	 * @param encoderInitialPosition the initial position of the encoder
	 */
	void EnableControl(double encoderInitialPosition = 0.0);

	/*!
	 * @brief Set ramp rate of voltage output
	 * @param rampRate ramp rate
	 */
	void SetVoltageRampRate(double rampRate);

	/*!
	 * @brief Set number of encoder codes per revolution
	 * @param codesPerRev codes per revolution
	 */
	void ConfigEncoderCodesPerRev(UINT16 codesPerRev);

	/*!
	 * @brief Set number of potentiometer turns
	 * @param turns number of potentiometer turns
	 */
	void ConfigPotentiometerTurns(UINT16 turns);

	/*!
	 * @brief Sets soft position limits
	 * @param forwardLimitPosition forward soft position limit
	 * @param reverseLimitPosition reverse soft position limit
	 */
	void ConfigSoftPositionLimits(double forwardLimitPosition,
			double reverseLimitPosition);

	/*!
	 * @brief Disables soft position limits
	 */
	void DisableSoftPositionLimits();

	/*!
	 * @brief Disables control
	 */
	void DisableControl();

	/*!
	 * @brief Sets maximum output voltage
	 * @param voltage the voltage
	 */
	void ConfigMaxOutputVoltage(double voltage);

	/*!
	 * @brief Configure fault time
	 * @param faultTime fault time
	 */
	void ConfigFaultTime(float faultTime);

	/*!
	 * @brief Configure expiration time
	 * @param timeout expiration timeout
	 */
	void SetExpiration(float timeout);

	/*!
	 * @brief Set the new position reference
	 * @param reference the new reference
	 */
	void SetPositionReference(CANJaguar::PositionReference reference);

	/*!
	 * @brief Determines whether or not current should be collected
	 * @param shouldCollect whether current should be collected
	 */
	void ShouldCollectCurrent(bool shouldCollect);

	/*!
	 * @brief Determines whether or not the analog value should be collected
	 * @param shouldCollect whether teh analog value should be collected
	 */
	void ShouldCollectPotValue(bool shouldCollect);

	/*!
	 * @brief Gets the current through the motor
	 * @return the current in amperes
	 */
	float GetCurrent();

	/*!
	 * @brief Gets the analog value through the motor
	 * @return the potentiometer value
	 */
	float GetPotValue();

	/*!
	 * @brief Wraps the communication task for threading
	 * @param proxiedCANJaguarPointer
	 * @return 0
	 */
	static int CommTaskWrapper(UINT32 proxiedCANJaguarPointer);

	/*!
	 * @brief Synchronizes the Jaguar with the cRIO status
	 */
	void CommTask();

	/*!
	 * @brief Initializes communications
	 */
	void BeginComm();

	/*!
	 * Simple wrapper for GetFaults() == 0
	 * @return true if no faults
	 */
	bool StatusOK();

	/*!
	 * S@briefet the game state
	 * @param state
	 */
	static void SetGameState(GameState state);

	/*!
	 * @param Clear the cache
	 */
	void ResetCache();
};

#endif /* PROXIED_CAN_JAGUAR_H_ */
