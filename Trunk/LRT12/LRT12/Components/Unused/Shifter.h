#ifndef SHIFTER_H_
#define SHIFTER_H_

#include "../Component.h"
#include "../..\Config\Configurable.h"
#include "../../Log/Loggable.h"

class Config;
class DriveEncoders;
class LRTServo;
class VirtualLRTServo;

class Shifter: public Component, public Configurable, public Loggable
{
private:
	LRTServo* leftServo, *rightServo;

	DriveEncoders& encoders;

	string config_section;

	//only let servo become disabled after a delay.
	//when Hitec HS322 servo is disabled, it jumps from it's last set point. -dg
	int servoDisableTimer;
	int servoDisableDelay; // 5 sec

	int lowGearServoValLeft;
	int highGearServoValLeft;
	int lowGearServoValRight;
	int highGearServoValRight;

public:
	/*!
	 * Constructs a shifter
	 */
	Shifter();
	virtual ~Shifter();

	void virtual Output();
	virtual void Configure();
	virtual string GetName();
	virtual void log();
};

#endif //SHIFTER_H_
