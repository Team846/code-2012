#ifndef BPD_ACTION_H
#define BPD_ACTION_H
#include "ActionData.h"

namespace ACTION
{
	enum completionStatus
	{ UNSET = 0, IN_PROGRESS = 1, SUCCESS = 2, FAILURE = 3, ABORTED = 4};
	
	char* const status_string[4 + 1] = 
	{ "Unset", "In Progress", "Success", "Failure", "Aborted" };

	namespace WEDGE
	{
		enum state
		{
			IDLE = 1, PRESET_BOTTOM = 2, PRESET_TOP = 3
		};
		char* const state_string[6 + 1] =
		{"ERROR", "IDLE", "Preset_Bottom", "Preset_Top"};
	}
}

struct BPDAction
{
    ACTION::WEDGE::state state;
    ACTION::completionStatus completion_status;
};
#endif //BPDACTION_H
