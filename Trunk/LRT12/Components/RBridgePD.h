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
    AsyncCANJaguar* m_arm_esc;
#ifdef VIRTUAL
    VirtualPot* m_arm_pot;
#else
    AnalogChannel* m_arm_pot;
#endif

    int m_prev_state;

    double maxPosition, minPosition, midPosition, midPositionDeadband;
    double maxPowerUp, powerRetainUp, powerDown, midPGain;
    double midPowerUp, midPowerDown;
    double pGainDown, pGainUp, pGainMid;

    int timeoutCycles;
    int m_cycle_count;

    enum {BOTTOM = 1, TOP = 2};
    bool m_preset_mode;

    int m_pulse_count;
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
