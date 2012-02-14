#ifndef LAUNCHER_ACTION_H_
#define LAUNCHER_ACTION_H_

#include "ActionData.h"

namespace ACTION
{
namespace LAUNCHER
{
/*!
 * @brief Keeps track of the gearbox state
 */
enum launcherState
{
	DISABLED, RUNNING
};
}
}

struct LauncherAction
{
	ACTION::LAUNCHER::launcherState state;
	bool atSpeed;
	double speed;
};

#endif
