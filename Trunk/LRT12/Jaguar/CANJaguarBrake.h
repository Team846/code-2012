#ifndef CAN_JAGUAR_BRAKE_H_
#define CAN_JAGUAR_BRAKE_H_

#include "CANJaguar.h"
#include "AsyncCANJaguar.h"
#include "..\Util\Util.h"

/*!
 * @brief Class that uses neutral mode to implement levels of braking power
 */

class CANJaguarBrake
{
	AsyncCANJaguar& m_jaguar;
	int m_cycle_count;
	int m_brake_level;

public:
	/*!
	 * @brief constructs a brake
	 * @param jaggie
	 */
	CANJaguarBrake(AsyncCANJaguar& jaggie);

	/*!
	 * @brief Sets braking power
	 * @param brakeAmount [0, 8]; 0 is no braking and 8 is max braking
	 */
	virtual void SetBrake(int brakeAmount);

	/*!
	 * @brief does nothing if setpoint is non-zero
	 * @brief applies brake based on call to SetBrake
	 */
	virtual void ApplyBrakes();

	/*!
	 * @brief braking is dependent on speed; higher speeds will result in
	 * @brief more dramatic braking. speed is not considered in this method
	 */
	virtual void SetBrakeMax()
	{
		SetBrake(8);
	}

	/*!
	 * @brief turns brake off.
	 */
	virtual void SetBrakeOff()
	{
		SetBrake(0);
	}
};

#endif /* CAN_JAGUAR_BRAKE_H_ */
