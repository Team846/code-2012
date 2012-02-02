#ifndef BPD_ACTION_H
#define BPD_ACTION_H
#include "ActionData.h"

namespace ACTION
{
	enum eCompletionStatus
	{ UNSET = 0, IN_PROGRESS = 1, SUCCESS = 2, FAILURE = 3, ABORTED = 4};
	
	char* const status_string[4 + 1] = 
	{ "ERROR", "In Progress", "Success", "Failure", "Aborted" };

	namespace BPD
	{
		enum eStates
		{
			IDLE = 1, PRESET_BOTTOM = 2, PRESET_TOP = 3,
			PRESET_MIDDLE = 4,  MANUAL_DOWN = 5, MANUAL_UP = 6
		};
		char* const state_string[6 + 1] =
		{"ERROR", "IDLE", "Preset_Bottom", "Preset_Top", "Preset_Middle", "Manual_Down", "Manual_Up"};
	}
}

struct BPDAction
{
    ACTION::BPD::eStates state;
    ACTION::eCompletionStatus completion_status;
};
#endif //BPDACTION_H
