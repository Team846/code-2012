#include "Component.h"

#include "ConfigLoader.h"
#include "Drivetrain.h"
#include "Shifter.h"

Component::ComponentData Component::createComponentData(
		bool RequiresEnabledState, int DIO)
{
	ComponentData ret;
	ret.RequiresEnabledState = false;
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
			ComponentWithData(new Shifter(), createComponentData(true, 5)));
	components->push_back(
			ComponentWithData(new Drivetrain(), createComponentData(true, 1)));

	//sample initialization of components
	//    components->push_back(ComponentWithData(new Shifter(), createComponentData(true, 5)));
	//    components->push_back(ComponentWithData(new ModifiedDriveTrain(), createComponentData(true, 1)));

	return components;
}
