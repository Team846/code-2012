#ifndef BALL_FEED_ACTION_H_
#define BALL_FEED_ACTION_H_

#include "ActionData.h"

namespace ACTION
{
namespace BALLFEED
{
/*!
 * @brief Keeps track of the gearbox state
 */
enum collector_state
{
	NEUTRAL, COLLECTING, REJECTING
};
}
}

struct BallFeedAction
{
	ACTION::BALLFEED::collector_state collector_state;
	bool sweepArmOut;
};
#endif
