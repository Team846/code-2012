#include "ConfigLoader.h"
#include "..\Config\Config.h"
#include "..\Util\AsyncPrinter.h"
#include "..\ActionData\ConfigAction.h"

ConfigLoader::ConfigLoader() :
	Component()
{
	m_config = Config::GetInstance();
}

ConfigLoader::~ConfigLoader()
{

}

void ConfigLoader::Output()
{
	if (m_action->config->load)
	{
		AsyncPrinter::Printf("Loading Configuration\n");
		m_config->Load();
		m_action->config->load = false;
	}
	if (m_action->config->save)
	{
		AsyncPrinter::Printf("Saving Configuration\n");
		m_config->Save();
		m_action->config->save = false;
	}
	if (m_action->config->apply)
	{
		AsyncPrinter::Printf("Applying Configuration\n");
		m_config->ConfigureAll();
		m_action->config->apply = false;
	}

#if 1
	static int cycleCount = 0;
	if (++cycleCount % 50 == 0)
		m_config->CheckForFileUpdates();
#endif
}

string ConfigLoader::GetName()
{
	return "ConfigLoader";
}
