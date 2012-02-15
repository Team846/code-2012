#ifndef BALL_FEED_ACTION_H_
#define BALL_FEED_ACTION_H_

#include "ActionData.h"

namespace ACTION
{
namespace BALLFEED
{
/*!
 * @brief Keeps track of the ball collector state
 */
enum collector_state
{
	NEUTRAL, COLLECTING, REJECTING
};

/*!
 * @brief Controls ball feeder state
 */
enum feeder_state
{
	FEEDING, HOLDING, PURGING
};
}
}

struct BallFeedAction
{
	ACTION::BALLFEED::collector_state collector_state;
	ACTION::BALLFEED::feeder_state feeder_state;
	bool sweepArmOut;
	bool attemptToLoadRound;
};
#endif
