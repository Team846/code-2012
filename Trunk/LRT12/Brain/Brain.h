#ifndef BRAIN_H_
#define BRAIN_H_

#include <WPILib.h>
#include "../ActionData/ActionData.h"
#include "../DriverInputs/DebouncedJoystick.h"

/**
 * @brief Processes inputs from {Autonomous|Joystick} and sets appropriate values in ActionData
 *  
 * @author Brian Axelrod
 * @author Robert Ying
 */

#warning incomplete

class Brain
{
public:
	/*!
	 * @brief Constructs the brain.
	 */
	Brain();
	
	/*!
	 * @brief starts teleop task
	 */
	void startTeleop();
	
	/*!
	 * @brief starts the auton task
	 */
	void startAuton();
private:
	/*!
	 * @brief processes the input
	 */
	void process();
	
	/*!
	 * @brief the autonomous task
	 */
	void autonTask();
	
	/*!
	 * @brief the teleop task
	 */
	void teleopTask();
	
	/*!
	 * 
	 * @param BrainPtr
	 * @return 0
	 */
	static int autonTaskEntryPoint(uint32_t autonTaskPtr);

	/*!
	 * 
	 * @param BrainPtr
	 * @return 0
	 */
	static int teleopTaskEntryPoint(uint32_t autonTaskPtr);
	
	Task m_teleop_task;
	Task m_auton_task;
	semaphore* actionSemaphore;
	
	ActionData *actionData;
	
	DriverStation *m_ds;
	DebouncedJoystick m_driver_stick;
	DebouncedJoystick m_operator_stick;
	
	uint32_t missedPackets;
	bool isTeleop;
};

#endif // BRAIN_H_
