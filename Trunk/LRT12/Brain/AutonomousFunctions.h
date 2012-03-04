#ifndef AUTON_FUNC_H_
#define AUTON_FUNC_H_

#include "WPILib.h"
#include "../Config/Configurable.h"
#include "../Config/Config.h"
#include "../Log/Loggable.h"
#include "../Config/RobotConfig.h"
#include "../ActionData/ActionData.h"
#include "../ActionData/DriveTrainAction.h"
#include "../ActionData/AutonomousAction.h"
#include "../ActionData/ShifterAction.h"
#include "../ActionData/IMUData.h"
#include "../ActionData/CameraData.h"

class PID;

/*!
 * Contains assist functions for autonomous and teleoperated usage
 * @author Robert Ying
 */
class AutonomousFunctions: public Configurable, public Loggable
{
public:
	/*!
	 * Destroys an AutonomousFunctions object
	 */
	~AutonomousFunctions();

	/*!
	 * Gets singleton instance
	 * @return pointer to singleton
	 */
	static AutonomousFunctions * getInstance();

	/*!
	 * Configures from config file
	 */
	virtual void Configure();

	/*!
	 * Logs values
	 */
	virtual void log();

	/*!
	 * Releases semaphore
	 */
	void releaseSemaphore();

	/*!
	 * Starts background task
	 */
	void startBackgroundTask();

	/*!
	 * Stops background task
	 */
	void stopBackgroundTask();

private:
	/*!
	 * Constructs an AutonomousFunctions object
	 */
	AutonomousFunctions();

	/*!
	 * Entry point for the task
	 */
	static void taskEntryPoint();

	/*!
	 * Actual task
	 */
	void task();

	/*!
	 * Iterative method to balance the bridge
	 */
	void bridgeBalance();

	/*!
	 * Alternate iterative method to balance the bridge
	 */
	void alternateBridgeBalance();

	/*!
	 * Iterative method to track the key
	 */
	void keyTrack();

	/*!
	 * Iterative method to align the shooter
	 */
	void autoAlign();

	static AutonomousFunctions * m_instance;

	SEM_ID m_task_sem;
	Task * m_task;

	ACTION::AUTONOMOUS::state m_pstate;

	std::string m_name;
	bool m_is_running;

	uint32_t m_counter;
	uint32_t m_timeout_bridgebalance, m_timeout_keytrack, m_timeout_autoalign;

	ActionData * m_action;
	PID *m_bridgebalance_pid;
	double m_keytrack_forward_rate;
	double m_align_turn_rate;
	double m_bridgebalance_setpoint, m_align_setpoint;
	double m_bridgebalance_angular_rate_threshold;
	double m_bridgebalance_threshold, m_keytrack_threshold, m_align_threshold;
	
	bool m_hit_key_flag;

	DISALLOW_COPY_AND_ASSIGN( AutonomousFunctions);
};

#endif
