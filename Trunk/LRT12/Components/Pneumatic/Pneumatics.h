#ifndef PNEUMATICS_H_
#define PNEUMATICS_H_

#include "WPILib.h"
#include "DoubleSolenoid.h"
#include "Compressor.h"
#include "../../Config/RobotConfig.h"
#include "../../Config/Configurable.h"
#include "../../Config/Config.h"
#include "../../Log/Loggable.h"
#include "../../Util/AsyncProcess.h"

typedef struct
{
	DoubleSolenoid * solenoid;
	int counter;
	bool state;
	bool pulsed;
} PulsedSolenoid;

class Pneumatics: public AsyncProcess, public Configurable, public Loggable
{
public:
	/*!
	 * Destructor
	 */
	virtual ~Pneumatics();

	/*!
	 * Gets an instance of Pneumatics
	 * @return pointer to instance
	 */
	static Pneumatics * getInstance();

	/*!
	 * Sets the shifter solenoids
	 * @param on
	 */
	void setShifter(bool on, bool force = false);

	/*!
	 * Sets the ball collector arm
	 */
	void setBallCollector(bool on, bool force = false);

	/*!
	 * Sets the trajectory selection
	 */
	void setTrajectory(bool on, bool force = false);

	/*!
	 * Sets the status of the latch
	 * interlocked with setPressurePlate
	 * @param on
	 */
	void setLatch(bool on, bool force = false);

	/*!
	 * Sets the status of the pressure plate
	 * interlocked with setLatch
	 * @param on
	 */
	void setPressurePlate(bool on, bool force = false);
	
	/*!
	 * Sets the state of the compressor
	 * @param on
	 */
	void setCompressor(bool on);

	/*!
	 * Configures values
	 */
	virtual void Configure();

	/*!
	 * Logs value
	 */
	virtual void log();

	/*!
	 * Actual work
	 */
	void work();

private:
	/*!
	 * Constructor
	 */
	Pneumatics();

	/*!
	 * Pulses a solenoid
	 * @param ptr the pulsed solenoid
	 */
	void pulse(PulsedSolenoid * ptr);

	PulsedSolenoid m_shifter, m_ballcollector, m_trajectory, m_shared;

	static Pneumatics* m_instance;

	std::string m_name;

	int m_pulse_length;
	bool m_mutex;

	Compressor *m_compressor;

	DISALLOW_COPY_AND_ASSIGN(Pneumatics);

};

#endif
