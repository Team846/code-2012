#ifndef LAUNCHER_H_
#define LAUNCHER_H_

#include "Component.h"
#include "../Config/Configurable.h"
#include "../Config/RobotConfig.h"
#include "../Config/Config.h"
#include "../Log/Loggable.h"
#include "../ActionData/LauncherAction.h"
#include "../Jaguar/AsyncCANJaguar.h"
#include "../Util/PID.h"
#include "Counter.h"

/*!
 * This class controls the ball launcher
 * @author Robert Ying
 */

class Launcher: public Component, public Configurable, public Loggable
{
public:
	/*!
	 * Constructs a launcher object
	 */
	Launcher();

	/*!
	 * Destructs a launcher object
	 */
	~Launcher();

	// overloaded methods
	virtual void Output();
	virtual void Configure();
	virtual std::string GetName();
	virtual void log();
private:
	PID m_pid;
	AsyncCANJaguar *m_roller;
	Counter *m_enc;
	std::string m_name;
};
#endif
