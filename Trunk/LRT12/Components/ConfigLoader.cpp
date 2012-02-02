#include "ConfigLoader.h"
#include "..\Config\Config.h"
#include "..\Util\AsyncPrinter.h"
#include "..\ActionData\ConfigAction.h"

ConfigLoader::ConfigLoader() :
	Component(), m_config(Config::GetInstance())
{
}

ConfigLoader::~ConfigLoader()
{

}

void ConfigLoader::Output()
{
	if (action->config->load)
	{
		AsyncPrinter::Printf("Loading Configuration\n");
		m_config->Load();
	}
	if (action->config->save)
	{
		AsyncPrinter::Printf("Saving Configuration\n");
		m_config->Save();
	}
	if (action->config->apply)
	{
		AsyncPrinter::Printf("Applying Configuration\n");
		m_config->ConfigureAll();
	}
}

string ConfigLoader::GetName()
{
	return "ConfigLoader";
}
