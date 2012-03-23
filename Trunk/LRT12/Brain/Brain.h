#ifndef BRAIN_H_
#define BRAIN_H_

#include <WPILib.h>
#include "../ActionData/ActionData.h"
#include "../DriverInputs/DebouncedJoystick.h"
#include "../DriverInputs/InputParser.h"
#include "../Util/SyncProcess.h"

/**
 * @brief Processes inputs from {Autonomous|Joystick} and sets appropriate values in ActionData
 *  
 * @author Brian Axelrod
 * @author Robert Ying
 */

class Brain: public SyncProcess
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

	void work();

private:
	ActionData *action;
	InputParser *m_inputs;

	DriverStation *m_ds;

	uint32_t missedPackets;
};

#endif // BRAIN_H_
