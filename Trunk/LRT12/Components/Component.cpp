#include "Component.h"

#include "ConfigLoader.h"
#include "Drivetrain.h"
#include "Launcher.h"
#include "Shifter.h"
#include "BallCollector.h"
#include "BallFeeder.h"
#include "Wedge.h"
#include "PneumaticShifter.h"
#include "../Config/RobotConfig.h"

Component::ComponentData Component::createComponentData(
		bool RequiresEnabledState, int DIO)
{
	ComponentData ret;
	ret.RequiresEnabledState = RequiresEnabledState;
	ret.DS_DIOToDisableComponent = DIO;
	return ret;
}

list<Component::ComponentWithData>* Component::CreateComponents()
{
	list<ComponentWithData>* components = new list<ComponentWithData> ();

	components->push_back(
			ComponentWithData(
					new ConfigLoader(),
					createComponentData(false, ComponentData::NO_DS_DISABLE_DIO)));
	components->push_back(
			ComponentWithData(new Drivetrain(), createComponentData(true, RobotConfig::DRIVER_STATION::DRIVETRAIN)));
	
	//Bug in pneumatic shifter
//	components->push_back(
//			ComponentWithData(new PneumaticShifter(), createComponentData(true, RobotConfig::DRIVER_STATION::SHIFTER)));
	components->push_back(
			ComponentWithData(new Launcher(), createComponentData(true, RobotConfig::DRIVER_STATION::LAUNCHER)));
//	components->push_back(
//			ComponentWithData(new BallCollector(), createComponentData(true, 4)));
//	components->push_back(
//			ComponentWithData(new Wedge(), createComponentData(true, 5)));
//	components->push_back(
//			ComponentWithData(new BallFeeder(), createComponentData(true, RobotConfig::DRIVER_STATION::BALLFEEDER)));

	//sample initialization of components
	//    components->push_back(ComponentWithData(new Shifter(), createComponentData(true, 5)));
	//    components->push_back(ComponentWithData(new ModifiedDriveTrain(), createComponentData(true, 1)));

	return components;
}
