#ifndef LOGGABLE_H_
#define LOGGABLE_H_

#include "Log.h"

#define LOGGING_ENABLED 1

#if LOGGING_ENABLED
#include "WPILib/SmartDashboard/SmartDashboard.h"
#endif

/*!
 * @brief The base class for all classes that log values
 */
class Loggable
{
public:
	/*!
	 * @brief registers the loggable as a listener with the Log class.
	 */
	Loggable();
	virtual ~Loggable();

	/*!
	 * @brief Called to log new data
	 */
	virtual void log() = 0;

	/*!
	 * Whether or not to log this loggable
	 * @return
	 */
	bool shouldLog();

	/*!
	 * Enables logging for this loggable
	 */
	void enableLog();

	/*!
	 * Disables logging for this loggable
	 */
	void disableLog();

private:
	bool m_should_log;
};

#endif
