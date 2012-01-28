#ifndef LAUNCHER_H_
#define LAUNCHER_H_

#include "Component.h"
#include "../Config/Configurable.h"
#include "../Config/RobotConfig.h"
#include "../Config/Config.h"
#include "../Log/Loggable.h"
#include "../ActionData/LauncherAction.h"
#include "../Jaguar/AsyncCANJaguar.h"

/*!
 * @brief Component that handles the blauncher
 * @author Brent Yi
 */

class BLauncher: public Component, public Configurable, public Loggable
{
public:
	/*!
	 * Constructs a blauncher object
	 */
	BLauncher();
	/*!
	 * Destructs a blauncher object
	 */
	~BLauncher();
	virtual void Configure();
	virtual void Output();
	virtual std::string GetName();
	virtual void log();
private:
	ACTION::LAUNCHER::launcherState m_prevstate;
	AsyncCANJaguar *m_top_roller, *m_bottom_roller;
	std::string m_name;
	float m_pid_top[3], m_pid_bottom[3];
	const static int proportional;
	const static int integral;
	const static int derivative;
};
#endif
