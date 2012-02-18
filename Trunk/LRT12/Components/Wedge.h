#ifndef WEDGE_H_
#define WEDGE_H_

#include <math.h>
#include "Component.h"
#include "..\Config\Configurable.h"
#include "..\Config\RobotConfig.h"
#include "..\Jaguar\AsyncCANJaguar.h"
#include "..\Config\Config.h"
#include "..\Util\AsyncPrinter.h"
#include "..\ActionData\BPDAction.h"
#include "../Log/Loggable.h"
#include "Relay.h"

class Config;

/*!
 * Class that handles the window motor for the ledge.
 * When the hardware design is finalized, this will also have to handle
 * locking and unlocking the mechanical interlock
 * @author Robert Ying
 */

class Wedge: public Component, public Configurable
{
private:
	std::string m_name;

	Relay* m_spike;
	DigitalInput *m_bottomlimit, *m_toplimit;

	int m_pulse_down, m_pulse_up;
	int m_ctr;

	ACTION::BPD::eStates m_lastState;

public:
	/*!
	 * Constructs a wedge
	 */
	Wedge();
	virtual ~Wedge();

	/*!
	 * Configures the wedge
	 */
	virtual void Configure();

	/*!
	 * Drives the wedge according to the action data
	 */
	virtual void Output();

	/*!
	 * Returns the name of the wedge
	 * @return name
	 */
	virtual string GetName();
	
	/*!
	 * Logs data
	 */
	virtual void log();
};

#endif