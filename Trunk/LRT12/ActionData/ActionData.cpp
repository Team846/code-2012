#include "ActionData.h"
#include "ConfigAction.h"


ActionData* ActionData::instance = NULL;

ActionData* ActionData::GetInstance()
{
    if(instance == NULL)
        instance = new ActionData();
    return instance;
}

ActionData::ActionData()
{
    // used to abort movements
    master.abort = false;

    wasDisabled = true;

    config = (ConfigAction*) malloc(sizeof(ConfigAction));
    config->load = false;
    config->save = false;
    config->apply = false;
}

ActionData::~ActionData()
{
    free(config);
}
