#ifndef DAVIDLAUNCHER_H_
#define DAVIDLAUNCHER_H_

#include "Component.h"
#include "../Config/Configurable.h"
#include "../Config/RobotConfig.h"
#include "../Config/Config.h"
#include "../Log/Loggable.h"
#include "../ActionData/LauncherAction.h"
#include "../Jaguar/AsyncCANJaguar.h"

/*!
 * Ball launcher control
 * @author David Yan
 */

class DavidLauncher: public Component, public Configurable, public Loggable
{
public:
	/*!
	 * Construct Launcher
	 */
	DavidLauncher();
	/*!
	 * Destruct Launcher
	 */
	~DavidLauncher();
	
	virtual void Output();
	virtual void Configure();
	virtual std::string GetName();
	virtual void log();
	
private: 
	ACTION::LAUNCHER::launcherState m_prevstate;
	AsyncCANJaguar *m_top_roller, *m_bottom_roller;
	std::string m_name;
	float m_pid_top[3], m_pid_bottom[3];
	const static int PROPORTIONAL = 0, DERIVATIVE = 1, INTEGRAL = 2;
};
#endif
