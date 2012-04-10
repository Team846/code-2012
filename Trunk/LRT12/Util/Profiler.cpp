#include "Profiler.h"

Profiler* Profiler::instance = NULL;

Profiler& Profiler::GetInstance()
{
	if (instance == NULL)
	{
		instance = new Profiler();
		FILE * fp = fopen(PROFILER_FILE_NAME, "a");
		if (fp)
		{
			fprintf(fp, "\r\n\r\nRobot started!\r\n\r\n");
			fclose(fp);
		}
	}
	return *instance;
}

Profiler::Profiler()
{
	cycleIndex = 0;
	AddToSingletonList();
}

template<class PairT> struct SortBySecondValue
{
	bool operator()(const PairT& lhs, const PairT& rhs) const
	{
		return lhs.second > rhs.second;
	}
};

void Profiler::PrintProfiledData(ProfiledData * p)
{
	double reportStart = Timer::GetFPGATimestamp();
	AsyncPrinter::Printf("----------------------\n");
	AsyncPrinter::Printf("PROFILER (%d cycles)\n", reportPeriod);
	AsyncPrinter::Printf("%s\r\n", p->enabled ? "Enabled" : "Disabled");

	typedef map<string, double>::value_type paired;
	typedef set<paired, SortBySecondValue<paired> > SetSortedBySecond;
	SetSortedBySecond vals;

	for (map<string, double>::iterator it = p->loggedMaxs.begin(); it
			!= p->loggedMaxs.end(); ++it)
	{
		vals.insert(paired(it->first, it->second / p->loggedCounts[it->first])); // to sort by means
		//		vals.insert(paired(it->first, it->second));
	}

	int i = 0;
	for (SetSortedBySecond::iterator it = vals.begin(); it != vals.end(); ++it)
	{
		double min = p->loggedMins[it->first];
		double max = p->loggedMaxs[it->first];
		int count = p->loggedCounts[it->first];
		double mean = p->loggedSums[it->first] / count;

		AsyncPrinter::Printf("| %-30s ~%.2f [%.2f-%.2f] x%d\n",
				it->first.c_str(), mean, min, max, count);

		++i;
		if (i > reportLimit)
			break;
	}

	double reportTime = (Timer::GetFPGATimestamp() - reportStart) * 1000;

	AsyncPrinter::Printf("End report (%.2f ms)\n", reportTime);
}

void Profiler::PrintProfiledDataToFile(const char * filename, ProfiledData * p,
		bool console)
{
	double reportStart = Timer::GetFPGATimestamp();

	if (justEnabled)
	{
		AsyncPrinter::Printf("\r\nJust Enabled\r\n\r\n");
	}

	AsyncPrinter::Printf("----------------------\n");
	AsyncPrinter::Printf("PROFILER to FILE (%d cycles)\n", reportPeriod);
	AsyncPrinter::Printf("%s\r\n", p->enabled ? "Enabled" : "Disabled");

	FILE * fp = fopen(filename, "a");
	if (fp)
	{
		if (justEnabled)
		{
			fprintf(fp, "\r\nJust Enabled\r\n\r\n");
		}
		fprintf(fp, "----------------------\r\n");
		fprintf(fp, "PROFILER to FILE (%d cycles)\r\n", reportPeriod);
		fprintf(fp, "%s\r\n", p->enabled ? "Enabled" : "Disabled");

		typedef map<string, double>::value_type paired;
		typedef set<paired, SortBySecondValue<paired> > SetSortedBySecond;
		SetSortedBySecond vals;

		for (map<string, double>::iterator it = p->loggedMaxs.begin(); it
				!= p->loggedMaxs.end(); ++it)
		{
			vals.insert(
					paired(it->first, it->second / p->loggedCounts[it->first])); // to sort by means
			//		vals.insert(paired(it->first, it->second));
		}

		int i = 0;
		for (SetSortedBySecond::iterator it = vals.begin(); it != vals.end(); ++it)
		{
			double min = p->loggedMins[it->first];
			double max = p->loggedMaxs[it->first];
			int count = p->loggedCounts[it->first];
			double mean = p->loggedSums[it->first] / count;
			if (console)
			{
				fprintf(fp, "| %-30s ~%.2f [%.2f-%.2f] x%d\n",
						it->first.c_str(), mean, min, max, count);
				AsyncPrinter::Printf("| %-30s ~%.2f [%.2f-%.2f] x%d\n",
						it->first.c_str(), mean, min, max, count);
			}
			++i;
			if (i > reportLimit)
				break;
		}

		double reportTime = (Timer::GetFPGATimestamp() - reportStart) * 1000;

		AsyncPrinter::Printf("End report (%.2f ms)\n", reportTime);
		fprintf(fp, "End report (%.2f ms)\n", reportTime);
		fclose(fp);
	}
	else
	{
		AsyncPrinter::Printf("Could not open file %s\r\n", filename);
	}
}

void Profiler::startNewProcessCycle()
{
	++cycleIndex;

	bool dsEnabled = DriverStation::GetInstance()->IsEnabled();
	if (!current.enabled && dsEnabled && cycleIndex != 1)
	{
		justEnabled = true;
	}
	current.enabled = current.enabled || dsEnabled;

	work();
}

void Profiler::work()
{
	if (cycleIndex >= reportPeriod)
	{
#if PROFILE_TO_FILE
		if (current.enabled)
		{
			PrintProfiledDataToFile(PROFILER_FILE_NAME, &current, true);
		}
		else
		{
#endif
#if PROFILE_DO_PRINT
			PrintProfiledData(&current);
#endif
#if PROFILE_TO_FILE
		}
#endif
		justEnabled = false;
		cycleIndex = 0;
		ClearLogBuffer();
	}
}

void Profiler::Log(std::string name, double timeTaken)
{
	if (current.loggedCounts.find(name) == current.loggedCounts.end())
	{
		current.loggedCounts[name] = 1;
		current.loggedMins[name] = current.loggedMaxs[name]
				= current.loggedSums[name] = timeTaken;
	}
	else
	{
		++current.loggedCounts[name];
		if (timeTaken < current.loggedMins[name])
			current.loggedMins[name] = timeTaken;
		if (timeTaken > current.loggedMaxs[name])
			current.loggedMaxs[name] = timeTaken;
		current.loggedSums[name] += timeTaken;
	}
}

void Profiler::ClearLogBuffer()
{
	current.loggedCounts.clear();
	current.loggedMins.clear();
	current.loggedMaxs.clear();
	current.loggedSums.clear();
	current.enabled = false;
}
