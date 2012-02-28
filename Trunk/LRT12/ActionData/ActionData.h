#ifndef ACTION_DATA_H_
#define ACTION_DATA_H_
#include <WPILib.h>

/**
 * @brief Contains the actionable data (ex. setpoints) for the components.
 * @author Brian Axelrod
 * @author Robert Ying
 */

struct BPDAction;
struct ConfigAction;
struct DriveTrainAction;
struct ShifterAction;
struct LauncherAction;
struct BallFeedAction;
struct AutonomousAction;
struct IMU_Data;
struct camera;

class ActionData
{
private:
	static ActionData* instance;
protected:
	ActionData();
	/*!
	 * @brief defines the copy constructor as protected which prevents copies of this singleton from being made. 
	 */
	DISALLOW_COPY_AND_ASSIGN(ActionData);

public:
	static ActionData* GetInstance();
	/*!
	 * @brief Descrutor is called on program exit.
	 */
	~ActionData();

	/*!
	 * @brief contains commands and state info relevant to the configuration system. Is defined in ActionData\ConfigAction.h.
	 */
	ConfigAction* config;

	/*!
	 * @brief contains commands relevant to the drivetrain. Is defined in ActionData\DriveTrainAction.h
	 */
	DriveTrainAction* drivetrain;

	/*!
	 * @brief contains commands relevant to the shifter. Is defined in ActionData\ShifterAction.h
	 */
	ShifterAction* shifter;

	/*!
	 * @brief contains commands relevant to the launcher. Is defined in ActionData\LauncherAction.h
	 */
	LauncherAction* launcher;

	/*!
	 * @brief contains commands relevant to the wedge
	 */
	BPDAction* wedge;

	/*!
	 * @brief contains commands relevant to the ball feeding system
	 */
	BallFeedAction* ballfeed;

	/*!
	 * @brief contains autonomous commands
	 */
	AutonomousAction * auton;

	/*!
	 * @brief contains the imu data
	 */
	IMU_Data * imu;

	/*!
	 * @brief contains the camera data
	 */
	camera * cam;

	/*!
	 * @brief enables motors across the robot
	 */
	bool motorsEnabled;

	/*!
	 * @brief contains data that is relevant to all/many components.
	 */
	struct
	{
		bool abort;
	} master;

	/*!
	 * @brief flag that indicates if the robot was disabled the previous cycle.
	 */
	bool wasDisabled;
};
#endif // ACTIONDATA_H_
