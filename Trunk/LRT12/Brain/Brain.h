#ifndef BRAIN_H_
#define BRAIN_H_

#include <WPILib.h>
#include "../ActionData/ActionData.h"
#include "../DriverInputs/DebouncedJoystick.h"
#include "../DriverInputs/InputParser.h"

/**
 * @brief Processes inputs from {Autonomous|Joystick} and sets appropriate values in ActionData
 *  
 * @author Brian Axelrod
 * @author Robert Ying
 */

class Brain
{
public:
	/*!
	 * @brief Constructs the brain.
	 */
	Brain();

	/*!
	 * @brief Destructs the brain.
	 */
	~Brain();

	/*!
	 * @brief starts teleop task
	 */
	void startTeleop();

	/*!
	 * @brief gets the m_action_ptr semaphore
	 */
	void takeActionSem();

	/*!
	 * @brief releases the m_action_ptr semaphore
	 */
	void giveActionSem();

	/*!
	 * Starts the brain tasks
	 */
	void Brain::Start();

private:
	/*!
	 * @brief processes the input
	 */
	void process();

	/*!
	 * @brief the teleop task
	 */
	void teleopTask();

	/*!
	 * 
	 * @param BrainPtr
	 * @return 0
	 */
	static int taskEntryPoint(uint32_t autonTaskPtr);

	Task* m_teleop_task;
	SEM_ID actionSemaphore;

	ActionData *action;
	InputParser *m_inputs;

	DriverStation *m_ds;

	uint32_t missedPackets;
	bool isTeleop;
};

#endif // BRAIN_H_
