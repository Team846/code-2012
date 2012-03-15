#ifndef PNEUMATIC_SHIFTER_H_
#define PNEUMATIC_SHIFTER_H_

#include "Component.h"
#include "..\Config\Configurable.h"
#include "../Log/Loggable.h"

class Config;
class DriveEncoders;

class PneumaticShifter: public Component, public Configurable, public Loggable
{
private:
	DriveEncoders &encoders;
	string config_section;

public:
	/*!
	 * Constructs a PneumaticShifter
	 */
	PneumaticShifter();
	virtual ~PneumaticShifter();

	void virtual Output();
	void virtual Disable();
	virtual void Configure();
	virtual string GetName();
	virtual void log();
};

#endif //PneumaticShifter_H_
