#include "Loggable.h"
#include "Log.h"

Loggable::Loggable()
{
	Log::registerLoggable(this);
	enableLog();
}

void Loggable::enableLog()
{
	m_should_log = true;
}

void Loggable::disableLog()
{
	m_should_log = false;
}

bool Loggable::shouldLog()
{
	return m_should_log;
}

Loggable::~Loggable()
{

}
