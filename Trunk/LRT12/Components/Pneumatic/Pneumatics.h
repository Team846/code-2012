#ifndef PNEUMATICS_H_
#define PNEUMATICS_H_

#include "WPILib.h"
#include "DoubleSolenoid.h"
#include "../../Config/RobotConfig.h"
#include "../../Config/Configurable.h"
#include "../../Config/Config.h"
#include "../../Log/Loggable.h"

typedef struct
{
	DoubleSolenoid * solenoid;
	int counter;
	bool state;
	bool pulsed;
} PulsedSolenoid;

class Pneumatics: public Configurable, public Loggable
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
	void setShifter(bool on);

	/*!
	 * Sets the ball collector arm
	 */
	void setBallCollector(bool on);

	/*!
	 * Sets the trajectory selection
	 */
	void setTrajectory(bool on);

	/*!
	 * Sets the status of the latch
	 * interlocked with setPressurePlate
	 * @param on
	 */
	void setLatch(bool on);

	/*!
	 * Sets the status of the pressure plate
	 * interlocked with setLatch
	 * @param on
	 */
	void setPressurePlate(bool on);

	/*!
	 * Configures values
	 */
	virtual void Configure();

	/*!
	 * Logs value
	 */
	virtual void log();

	/*!
	 * Releases semaphore
	 */
	void releaseSemaphore();

	/*!
	 * Starts background task
	 */
	void startBackgroundTask();

	/*!
	 * Stops background task
	 */
	void stopBackgroundTask();

private:
	/*!
	 * Constructor
	 */
	Pneumatics();

	/*!
	 * Entry point for task
	 * @param ptr
	 */
	static void taskEntryPoint(UINT32 ptr);

	/*!
	 * Actual task
	 */
	void task();

	/*!
	 * Pulses a solenoid
	 * @param ptr the pulsed solenoid
	 */
	void pulse(PulsedSolenoid * ptr);

	PulsedSolenoid m_shifter, m_ballcollector, m_trajectory, m_shared;

	std::string m_name;

	int m_pulse_length;
	bool m_mutex;

	bool m_is_running;
	SEM_ID m_task_sem;
	Task * m_task;

	DISALLOW_COPY_AND_ASSIGN(Pneumatics);

};

#endif
