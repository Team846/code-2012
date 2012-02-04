#ifndef ARM_H_
#define ARM_H_

#include "Component.h"
#include "..\Config\Configurable.h"
using namespace std;

class AsyncCANJaguar;
class Config;
class VirtualPot;

/*!
 * \brief the component that handles the arm. It reads the sensor values and sends the motor commands based on inputs and commands. 
 */
class RBridgePD : public Component, public Configurable
{
private:
    Config *config;
    string configSection;
    AsyncCANJaguar* armEsc;
#ifdef VIRTUAL
    VirtualPot* armPot;
#else
    AnalogChannel* armPot;
#endif

    int oldState;

    double maxPosition, minPosition, midPosition, midPositionDeadband;
    double maxPowerUp, powerRetainUp, powerDown, midPGain;
    double midPowerUp, midPowerDown;
    double pGainDown, pGainUp, pGainMid;

    int timeoutCycles;
    int cycleCount;

    enum {BOTTOM = 1, TOP = 2};
    bool presetMode;

    int pulseCount;
    int hysteresis;

    const static double ARM_UP_THRESHOLD = 10;

public:
    RBridgePD();
    virtual ~RBridgePD();

    virtual void Configure();
    virtual void Output();

    virtual string GetName();
};

#endif
