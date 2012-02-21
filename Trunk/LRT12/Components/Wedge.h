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
#include "Solenoid.h"

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

	Relay *m_spike;

	int m_pulse_down, m_pulse_up;
	int m_ctr;
	int m_dither;

	ACTION::WEDGE::state m_lastState;

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
	 * Drives the wedge according to the m_action_ptr data
	 */
	virtual void Output();

	virtual void Disable();

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
