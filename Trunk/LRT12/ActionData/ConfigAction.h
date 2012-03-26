#ifndef CONFIG_ACTION_H
#define CONFIG_ACTION_H
#include "ActionData.h"

struct ConfigAction
{
	bool load;
	bool save;
	bool apply;

	ConfigAction()
	{
		load = save = apply = false;
	}
};
#endif //CONFIG_ACTION_H
