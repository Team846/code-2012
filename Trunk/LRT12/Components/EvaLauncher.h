//#ifndef EVALAUNCHER_H_
//#define EVALAUNCHER_H_
//
//#include "Component.h"
//#include "../Config/Configurable.h"
//#include "../Config/RobotConfig.h"
//#include "../Config/Config.h"
//#include "../Log/Loggable.h"
//#include "../ActionData/LauncherAction.h"
//#include "../Jaguar/AsyncCANJaguar.h"
//
///*!
// * @brief Component that handles the evalauncher
// * @author Eva Lomazov
// */
//class EvaLauncher: public Component, public Configurable, public Loggable
//{
//public:
//	/*!
//	 * @brief Constructs the evalauncher object
//	 */
//	EvaLauncher();
//
//	/*!
//	 * @brief Destructs the evalauncher object
//	 */
//	~EvaLauncher();
//
//	/*!
//	 * @brief Overloaded methods
//	 */
//	virtual void Configure();
//	virtual void Output();
//	virtual std::string GetName();
//	virtual void log();
//
//private:
//	ACTION::LAUNCHER::launcherState m_prevstate;
//	AsyncCANJaguar *m_top_roller, *m_bottom_roller;
//	std::string m_name;
//	double m_pid_top[3], m_pid_bottom[3];
//	const static int PROPORTIONAL = 0;
//	const static int INTEGRAL = 1;
//	const static int DERIVATIVE = 2;
//};
//
//#endif //EVALAUNCHER_H_
