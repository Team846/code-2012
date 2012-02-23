#ifndef DRIVETRAIN_H_
#define DRIVETRAIN_H_

#include "DriveTrain/CLDriveTrain.h"
#include "../Config/RobotConfig.h"
#include "../Config/Configurable.h"
#include "../Log/Loggable.h"
#include "../Jaguar/Esc.h"
#include "../Sensors/DriveEncoders.h"
#include "Component.h"

/*!
 * @brief Component that handles the drivetrain
 * @author Robert Ying
 */
class Drivetrain: public Component, Configurable, public Loggable
{
public:
	/*!
	 * @brief Constructs a drivetrain object
	 */
	Drivetrain();

	/*!
	 * @brief Destroys a drivetrain object
	 */
	~Drivetrain();

	virtual void Output();
	virtual void Disable();

	virtual std::string GetName();

	virtual void Configure();

	virtual void log();

private:
	std::string m_name;
	ClosedLoopDrivetrain m_drive_control;
	DriveEncoders& m_encoders;
	ESC *m_esc_left, *m_esc_right;
	int NUM_CYCLES_TO_SYNC;
};

#endif
