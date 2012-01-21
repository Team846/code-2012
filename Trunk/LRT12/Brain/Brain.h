#ifndef BRAIN_H_
#define BRAIN_H_

#include <WPILib.h>

/**
 * @brief Processes inputs from {Autonomous|Joystick} and sets appropriate values in ActionData
 *  
 * @author Brian Axelrod
 * @author Robert Ying
 */

#warning STUB_COMPLETE_

class Brain
{
public:
	Brain();
private:
	static int BrainTask(uint32_t brainTaskPtr);
	Task brainTask;
};

#endif // BRAIN_H_
