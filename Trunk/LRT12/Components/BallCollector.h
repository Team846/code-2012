#ifndef BALL_COLLECTOR_H_
#define BALL_COLLECTOR_H_

#include "Component.h"
#include "../Config/Config.h"
#include "../Config/Configurable.h"
#include "../Jaguar/AsyncCANJaguar.h"
#include "Solenoid.h"
#include "../Log/Loggable.h"

/*!
 * Controls the collection of balls from the front
 * @author Robert Ying
 */

class BallCollector: public Configurable, public Loggable, public Component
{
public:
	/*!
	 * Constructs a ball collector
	 */
	BallCollector();

	/*!
	 * Destructs a ball collector
	 */
	~BallCollector();

	/*!
	 * Turns the ball collector on or off depending on action data
	 */
	virtual void Output();
	
	/*!
	 * Disables ball collector
	 */
	virtual void Disable();
	
	/*!
	 * Configures values from Config
	 */
	virtual void Configure();

	/*!
	 * Returns name of the ball collector
	 * @return name
	 */
	virtual std::string GetName();

	/*!
	 * Logs data related to the ball collector
	 */
	virtual void log();

private:
	std::string m_name;
	std::string m_configsection;
	double m_fwd_duty, m_rev_duty;
	AsyncCANJaguar * m_roller;
	DoubleSolenoid * m_arm;
};

#endif
