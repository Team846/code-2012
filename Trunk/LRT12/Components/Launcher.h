#ifndef LAUNCHER_H_
#define LAUNCHER_H_

#include "Component.h"
#include "../Config/Configurable.h"
#include "../Config/RobotConfig.h"
#include "../Config/Config.h"
#include "../ActionData/LauncherAction.h"
#include "../Jaguar/AsyncCANJaguar.h"

/*!
 * This class controls the ball launcher
 * @author Robert Ying
 */

class Launcher: public Component, Configurable
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
private:
	ACTION::LAUNCHER::launcherState m_prevstate;
	AsyncCANJaguar *m_top_roller, *m_bottom_roller;
	std::string m_name;
	float m_pid_top[3], m_pid_bottom[3];
	const static int PROPORTIONAL = 0;
	const static int INTEGRAL = 1;
	const static int DERIVATIVE = 2;
};
#endif
