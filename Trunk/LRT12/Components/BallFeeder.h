#ifndef BALL_FEEDER_H_
#define BALL_FEEDER_H_

#include "Component.h"
#include "../Config/Config.h"
#include "../Config/Configurable.h"
#include "../Jaguar/AsyncCANJaguar.h"
#include "../Log/Loggable.h"
#include "Relay.h"

/*!
 * Controls the collection of balls from the front
 * @author Robert Ying
 */

class BallFeeder: public Configurable, public Loggable, public Component
{
public:
	/*!
	 * Constructs a ball collector
	 */
	BallFeeder();

	/*!
	 * Destructs a ball collector
	 */
	~BallFeeder();

	/*!
	 * Turns the ball feeder on or off depending on action data
	 */
	virtual void Output();

	/*!
	 * Configures values from Config
	 */
	virtual void Configure();

	/*!
	 * Returns name of the ball feed
	 * @return name
	 */
	virtual std::string GetName();

	/*!
	 * Logs data related to the ball collector
	 */
	virtual void log();

private:
	const static int FRONT = 0;
	const static int BACK = 1;
	std::string m_name;
	std::string m_configsection;
	double m_fwd_duty[2], m_rev_duty[2];
	AsyncCANJaguar * m_roller[2];
	Relay * m_pressure_plate;
};

#endif
