#ifndef AUTONOMOUS_ACTION_H_
#define AUTONOMOUS_ACTION_H_

#include "BPDAction.h"

namespace ACTION
{
namespace AUTONOMOUS
{
enum state
{
	TELEOP, BRIDGEBALANCE, KEYTRACK, AUTOALIGN
};
}
}

struct AutonomousAction
{
	ACTION::AUTONOMOUS::state state;
	ACTION::completionStatus completion_status;
};

#endif
