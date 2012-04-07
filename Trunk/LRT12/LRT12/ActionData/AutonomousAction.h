#ifndef AUTONOMOUS_ACTION_H_
#define AUTONOMOUS_ACTION_H_

#include "BPDAction.h"

namespace ACTION
{
namespace AUTONOMOUS
{
enum state
{
	TELEOP, BRIDGEBALANCE, KEYTRACK, AUTOALIGN, AUTON_MODE, POSITION_HOLD
};

enum direction
{
	CLOCKWISE = 1, COUNTER_CLOCKWISE = -1
};
}
}

struct AutonomousAction
{
	ACTION::AUTONOMOUS::state state;
	ACTION::completionStatus completion_status;
	ACTION::AUTONOMOUS::direction turnDir;

	AutonomousAction()
	{
		state = ACTION::AUTONOMOUS::TELEOP;
		completion_status = ACTION::UNSET;
		turnDir = ACTION::AUTONOMOUS::CLOCKWISE;
	}
};

#endif
