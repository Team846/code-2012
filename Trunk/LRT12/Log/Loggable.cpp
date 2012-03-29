#include "Loggable.h"
#include "Log.h"

#define ENABLE_LOG 1
Loggable::Loggable()
{
	Log::registerLoggable(this);
#if ENABLE_LOG
	enableLog();
#else
	disableLog();
#endif
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
