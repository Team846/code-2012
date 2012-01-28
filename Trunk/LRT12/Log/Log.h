#ifndef LRT_LOG_H_
#define LRT_LOG_H_

#include <vector>
#include <WPILib.h>

class Loggable;

/*!
 * Class to simplify logging
 * @author Robert Ying
 */
class Log
{
public:
	virtual ~Log();

	/*!
	 * Returns the singleton instance of a Log
	 * @return
	 */
	static Log& getInstance();

	/*!
	 * Registers a Loggable to be logged.
	 * @param loggable
	 */
	static void registerLoggable(Loggable * loggable);

	/*!
	 * Logs all values from all Loggables
	 */
	static void logAll();

private:
	Log();DISALLOW_COPY_AND_ASSIGN( Log);

	static Log* m_instance;
	static std::vector<Loggable*> m_loggables;
};

#endif