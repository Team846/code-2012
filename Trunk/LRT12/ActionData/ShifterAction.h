#ifndef SHIFTER_ACTION_H_
#define SHIFTER_ACTION_H_

#include "ActionData.h"

namespace ACTION
{
namespace GEARBOX
{
/*!
 * @brief Keeps track of the gearbox state
 */
enum eGearboxState
{
	LOW_GEAR = 1, HIGH_GEAR = 2
};
}
}

struct ShifterAction
{
	ACTION::GEARBOX::eGearboxState gear;
	bool force;
};

#endif
