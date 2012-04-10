#ifndef LRT_PROFILER_H_
#define LRT_PROFILER_H_

#include <map>
#include <string>
#include <sstream>
#include <iomanip>
#include "AsyncPrinter.h"
#include "../Config/RobotConfig.h"

#define PROFILE_DO_PRINT 0
#define PROFILE_TO_FILE 0

const char * const PROFILER_FILE_NAME = "profiler.txt";

/*!
 * Records profiled data for each cycle, as well as its enabled state
 */
struct ProfiledData
{
	map<string, int> loggedCounts;
	map<string, double> loggedMins;
	map<string, double> loggedMaxs;
	map<string, double> loggedSums;
	bool enabled;
};

/*!
 * Class to profile code runtimes
 *  Usage:
 *
 *  {
 *      ProfiledSection pf( "NameOfActivity" );
 *      // do something time consuming...
 *  }
 *  @author Robert Ying
 *  @author Karthik Viswanathan
 */
class Profiler: public SensorBase
{
public:
	/*!
	 * Number of cycles before publishing a report
	 */
	const static int reportPeriod = RobotConfig::LOOP_RATE * 5;

	/*!
	 * Number of items (max) within a report
	 */
	const static int reportLimit = 100;

	/*!
	 * Virtual destructor
	 */
	virtual ~Profiler()
	{
	}

	/*!
	 * Singleton instance
	 * @return reference to instance of Profiler
	 */
	static Profiler& GetInstance();

	/*!
	 * Starts a new process cycle
	 */
	void startNewProcessCycle();

	/*!
	 * Does the actual logic / filewriting IO
	 */
	void work();

	/*!
	 * Prints a set of data to console
	 * @param p pointer to ProfiledData
	 */
	void PrintProfiledData(ProfiledData * p);

	/*!
	 * Prints a set of data to a file
	 * @param filename filename of file to print to
	 * @param p pointer to ProfiledData
	 * @param console whether or not to print to console as well
	 */
	void PrintProfiledDataToFile(const char * filename, ProfiledData * p,
			bool console = false);

	/*!
	 * Logs a given component
	 * @param name Name of component
	 * @param timeTaken Time that component took
	 */
	void Log(std::string name, double timeTaken);

	/*!
	 * Clears logged values
	 */
	void ClearLogBuffer();

protected:
	/*!
	 * Protected constructor
	 */
	Profiler();DISALLOW_COPY_AND_ASSIGN(Profiler);

private:
	static Profiler* instance;

	ProfiledData current;

	int cycleIndex;

	bool justEnabled;
};

/*!
 * A section to be profiled
 */
class ProfiledSection
{
public:
	/*!
	 * Constructs a profiled section
	 * @param name name of the section 
	 */
	ProfiledSection(std::string name) :
		name(name), start(Timer::GetFPGATimestamp())
	{
	}

	/*!
	 * Destructs the profiled section and logs the time taken
	 */
	~ProfiledSection()
	{
		double ms = (Timer::GetFPGATimestamp() - start) * 1000;
		Profiler::GetInstance().Log(name, ms);
	}

protected:
	string name;
	double start;
};

/*!
 * Used to emulate a section without being a section
 */
class ProfilerHelper
{
public:
	/*!
	 * Constructs one of these things.
	 * For some reason doesn't call start()
	 */
	ProfilerHelper()
	{
		start = 0;
	}

	/*!
	 * Starts the timer
	 * @param name of the not-section
	 */
	void Start(string name)
	{
		name = name;
		start = Timer::GetFPGATimestamp();
	}

	/*!
	 * Ends the timer and logs the time taken
	 */
	void Finish()
	{
		double ms = (Timer::GetFPGATimestamp() - start) * 1000;
		Profiler::GetInstance().Log(name, ms);
	}

protected:
	string name;
	double start;
};

#endif
