#ifndef SHARED_CYLINDER_H_
#define SHARED_CYLINDER_H_

#include "WPILib.h"

class SharedSolenoid
{
public:

	virtual ~SharedSolenoid();
	/*!
	 * @brief The accessor method for the global instance of this class. 
	 */
	static SharedSolenoid* GetInstance();
	
	void EnablePressurePlate();
	void EnableLatch();
	void DisableLatch();
	void DisablePressurePlate();

private:
	DoubleSolenoid *m_solenoid;
	Task *m_task;
	static void taskEntry(uint32_t thingy);
	void task();
	
	bool pressurePlateEnabled, latchEnabled;
	
	SharedSolenoid();
	static SharedSolenoid* instance;

	DISALLOW_COPY_AND_ASSIGN(SharedSolenoid);
};

#endif
