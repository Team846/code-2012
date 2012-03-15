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
enum launcherConfiguration
{
	FENDER_SHOT_HIGH = 0,
	FENDER_SHOT_LOW = 1,
	KEY_SHOT_HIGH = 2,
	KEY_SHOT_LOW = 3,
	FAR_FENDER_SHOT_HIGH = 4,
	FAR_FENDER_SHOT_LOW = 5
};

char* const launcherConfigStr[6 + 1] =
{ "Fender High", "Fender Low", "Key High", "Key Low", "Far Fender High",
		"Far Fender Low" };
}
}

struct LauncherAction
{
	ACTION::LAUNCHER::launcherState state;
	bool atSpeed;
	double speed;
	ACTION::LAUNCHER::launcherConfiguration desiredTarget;
	bool isFenderShot;
	int ballLaunchCounter;
};

#endif
