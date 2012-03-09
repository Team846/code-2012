#ifndef BALL_FEED_ACTION_H_
#define BALL_FEED_ACTION_H_

#include "ActionData.h"

namespace ACTION
{
namespace BALLFEED
{

/*!
 * @brief Controls ball feeder state
 */
enum feeder_state
{
	FEEDING, HOLDING, PURGING, FREEZING
};
}
}

struct BallFeedAction
{
	ACTION::BALLFEED::feeder_state feeder_state;
	bool sweepArmOut;
	bool attemptToLoadRound;
};
#endif
