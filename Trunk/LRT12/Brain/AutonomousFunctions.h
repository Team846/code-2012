#ifndef AUTON_FUNC_H_
#define AUTON_FUNC_H_

#include "WPILib.h"
#include "../Config/Configurable.h"
#include "../Config/Config.h"
#include "../Util/PID.h"
#include "../Log/Loggable.h"
#include "../Config/RobotConfig.h"
#include "../ActionData/ActionData.h"
#include "../ActionData/DriveTrainAction.h"
#include "../ActionData/AutonomousAction.h"
#include "../ActionData/ShifterAction.h"
#include "../ActionData/IMUData.h"
#include "../ActionData/CameraData.h"
#include <queue>

/*!
 * Contains assist functions for autonomous and teleoperated usage
 * @author Robert Ying
 */
class AutonomousFunctions: public Configurable, public Loggable
{
public:
	/*!
	 * enum to keep track of stage
	 */
	enum autonomousStage
	{
		INIT = 1,//!< Initialize things
		KEY_TRACK = 2, //!< KEY_TRACK
		AIM = 3, //!< AIM
		SHOOT = 4, //!< SHOOT
		MOVE_BACK_INIT = 5, //!< MOVE_BACK
		MOVE_TO_FENDER_INIT = 6, //!< MOVE_BACK
		DROP_WEDGE = 7, //!< Drop the Wedge
		RAISE_WEDGE = 8, //!< Raise the Wedge
		WAIT_FOR_POSITION = 9, //!< MOVE_BACK
		DELAY_HALF_SEC = 10,
		ADJUSTABLE_DELAY = 11,
		DONE = 0
	//!< Finished
	};

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
	bool bridgeBalance();

	/*!
	 * Alternate iterative method to balance the bridge
	 */
	bool alternateBridgeBalance();

	/*!
	 * Another attempt to balance the bridge
	 */
	bool anotherBridgeBalance();
	
	/*!
	 * Iterative method to track the key
	 */
	bool keyTrack();

	/*!
	 * Iterative method to align the shooter
	 */
	bool autoAlign();

	/*!
	 * Our basic autonomous mode
	 */
	bool autonomousMode();

	/*!
	 * Load autonomous queue
	 */
	void loadQueue();

	/*!
	 * Advance autonomous queue
	 */
	void advanceQueue();

	/*!
	 * Gets name of autonomous stage
	 * @param a the autonomous stage
	 * @return pointer to string
	 */
	std::string getAutonomousStageName(autonomousStage a);

	const static int DRIVE_THEN_SHOOT_LENGTH = 10;
	const static autonomousStage DRIVE_THEN_SHOOT[DRIVE_THEN_SHOOT_LENGTH];

	const static int SHOOT_THEN_BRIDGE_LENGTH = 10;
	const static autonomousStage SHOOT_THEN_BRIDGE[SHOOT_THEN_BRIDGE_LENGTH];

	const static int BRIDGE_THEN_SHOOT_LENGTH = 10;
	const static autonomousStage BRIDGE_THEN_SHOOT[BRIDGE_THEN_SHOOT_LENGTH];

	std::queue<autonomousStage> m_auton_sequence;

	autonomousStage m_curr_auton_stage;

	static AutonomousFunctions * m_instance;

	SEM_ID m_task_sem;
	Task * m_task;

	ACTION::AUTONOMOUS::state m_last_state;

	std::string m_name;
	bool m_is_running;

	uint32_t m_counter;
	uint32_t m_timeout_bridgebalance, m_timeout_keytrack, m_timeout_autoalign;

	ActionData * m_action;
	PID m_bridgebalance_pid;
	PID m_auto_aim_pid;

	double m_keytrack_forward_rate;
	double m_max_align_turn_rate, m_min_align_turn_rate;
	double m_bridgebalance_setpoint, m_align_setpoint;
	double m_bridgebalance_angular_rate_threshold;
	double m_bridgebalance_threshold, m_keytrack_threshold, m_align_threshold;

	//double m_align_turned;
	
	bool m_hit_key_flag;

	bool hasStartedTipping;
	int m_direction;
	int m_haf_cyc_delay;
	int m_adj_cyc_delay, M_CYCLES_TO_DELAY;

	DISALLOW_COPY_AND_ASSIGN( AutonomousFunctions);
};

#endif
