#include "WPILib.h"
#include "IMU.h"

#include <taskLib.h>

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */
class RobotDemo: public SimpleRobot
{
	IMU *imu;
public:
	RobotDemo(void)
	{
		printf("Start constructor\n");
		imu = new IMU();
		printf("finish constructor\n");
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		while (true)
		{
		}
	}

	virtual void Disabled(void)
	{
		while (true)
		{
			//printf("start Disabled\n");
			imu->update();
			Wait(0.02);
			//			imu->printAll();
			//			Wait(1.0);
			//printf("end Disabled\n");
		}
	}
};

START_ROBOT_CLASS(RobotDemo)
;

