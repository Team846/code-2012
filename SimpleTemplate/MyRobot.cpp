#include "WPILib.h"
#include "WPILib/Encoder.h"

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */
class RobotDemo: public SimpleRobot {
	//	RobotDrive myRobot; // robot drive system
	Joystick stick; // only joystick

public:
	RobotDemo(void) :
		stick(1) // as they are declared above.
	{
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void) {
		Wait(2.0); //    for 2 seconds
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void) {
		Encoder enc_useless(9, 10);
		DigitalInput a(13);
		DigitalInput b(14);
		Encoder enc(a, b, true, Encoder::k2X);

		while (IsOperatorControl()) {
			//			printf("Encoder: Speed: [%f] [%f] Position: [%d] [%d]\r\n",
			//					test.GetRate(), test1.GetRate(), test.Get(), test1.Get());
#define p printf
#define j(x) (x.Get())
#define e(x) "Encoder: %d | %f ", x.Get(), x.GetRate()
#define d(x,y) "Inputs: %d %d", x.Get(), y.Get()
			p(d(a,b));
			p(" ");
			p(e(enc));
			p("\r\n");
			Wait(0.001); // wait for a motor update time
		}
	}
private:
};

START_ROBOT_CLASS(RobotDemo)
;

