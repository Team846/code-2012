#ifndef LAUNCHER_ACTION_H_
#define LAUNCHER_ACTION_H_

#include "ActionData.h"

namespace ACTION
{
namespace LAUNCHER
{
/*!
 * @brief Keeps track of the launcher state
 */
enum launcherState
{
	DISABLED, RUNNING
};
enum launcherSpeed{
	SLOW, MEDIUM, FASTEST
};
}
}

struct LauncherAction
{
	ACTION::LAUNCHER::launcherState state;
	bool atSpeed;
	double speed;
	ACTION::LAUNCHER::launcherSpeed desiredSpeed;
	double distance;
	bool topTrajectory;
};

#endif
