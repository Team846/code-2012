#include "Configurable.h"
#include "Config.h"

Configurable::Configurable()
:m_config(Config::GetInstance())
{
	Config::RegisterConfigurable(this);
}

Configurable::~Configurable()
{

}
