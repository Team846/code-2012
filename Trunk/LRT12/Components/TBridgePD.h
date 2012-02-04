#ifndef _TBRIDGEPD_H_
#define _TBRIDGEPD_H_

#include <math.h>
#include "Component.h"
#include "..\Config\Configurable.h"
#include "..\Config\RobotConfig.h"
#include "..\Jaguar\AsyncCANJaguar.h"
#include "..\Config\Config.h"
#include "..\Util\AsyncPrinter.h"
#include "..\ActionData\BPDAction.h"

using namespace std;

class ProxiedCANJaguar;
class Config;
class VirtualPot;

class TBridgePD : public Component, public Configurable
{
private:
	string m_name;
	string m_configSection;
	
	AsyncCANJaguar* m_bpdEsc;
	AnalogChannel* m_bpdPot;
	
	double m_minPosition;
	double m_maxPosition;
	
	double m_maxPowerUp;
	double m_powerDown;
	
	double m_pGainUp;
	double m_pGainDown;
	
	double m_powerRetainUp;
	
	ACTION::BPD::eStates m_lastState;
	
public:
	TBridgePD();
	virtual ~TBridgePD();
	
	virtual void Configure();
	virtual void Output();
	
	virtual string GetName();
};

#endif // _TBDRIDGEPD_H_
