#ifndef _CONFIG_LOADER_H
#define _CONFIG_LOADER_H

#include "Component.h"

class Config;

class ConfigLoader: public Component
{
public:
	ConfigLoader();
	virtual ~ConfigLoader();

	virtual void Output();
	virtual void Disable(){}
	virtual string GetName();
private:
	Config *m_config;
};

#endif
