#ifndef JAGUAR_TESTER_H_
#define JAGUAR_TESTER_H_

#include "WPILib.h"
#include "../Config/Config.h"

class JaguarTester
{
public:
	JaguarTester(int minIndex, int maxIndex)
	{
		if (Config::GetInstance()->Get<bool>("JagTest", "enabled", 0))
		{
			printf("Enable\n");
			Wait(10.0);
			printf("starting\n");
			for (int i = minIndex; i <= maxIndex; i++)
			{
				printf("Allocating Jag index %d\n", i);
				CANJaguar *jaggie = new CANJaguar(i);
				printf("setting to Jag %d\n", i);
				jaggie->Set(0.0);
				Wait(0.5);
				printf("Deleting Jag %d\n", i);
				delete jaggie;
				Wait(0.5);
			}
			printf("disable\n");
			Wait(1.0);
			Config::GetInstance()->Set<bool>("JagTest", "enabled", 0);
		}
		else
		{
			printf("Not testing Jaguars\n");
		}
	}
	
};

#endif

