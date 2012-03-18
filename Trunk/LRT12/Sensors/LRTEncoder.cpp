#include "LRTEncoder.h"

LRTEncoder::LRTEncoder(const char * name, UINT32 sourceA, UINT32 sourceB,
		double trim) :
	Encoder(sourceA, sourceB, false, CounterBase::k4X), trim(trim),
			m_name(name)
//    , useless(15 + count, , false, CounterBase::k1X)
{
}

LRTEncoder::~LRTEncoder()
{
	// nothing
}

void LRTEncoder::Start()
{
	Encoder::Stop();
	Encoder::Start();
	Encoder::SetMinRate(10);
}

double LRTEncoder::GetRate()
{
	// WPILib's GetRate() returns NaN if the rate is 0
	// Karthik Viswanathan and Brian Axelrod, January 29th, 2011
	double rate = Encoder::GetRate();
	if (Encoder::GetStopped() || rate != rate) // test if rate is NaN because NaN != NaN cuz WPILib is evil D:
		return 0.0;
	return rate;
}

INT32 LRTEncoder::Get()
{
	return (INT32) (Encoder::Get() * trim);
}

void LRTEncoder::log()
{
#if LOGGING_ENABLED
	SmartDashboard * sdb = SmartDashboard::GetInstance();
	std::string prefix = m_name + ": ";
	sdb->PutDouble((prefix + "Rate").c_str(), GetRate());
	sdb->PutInt((prefix + "Count").c_str(), Get());
#endif
}
