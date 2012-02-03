#include <math.h>
#include "RBridgePD.h"
#include "..\Config\Robotconfig.h"
#include "..\Jaguar\ASyncCANJaguar.h"
#include "..\Config\config.h"
#include "..\ActionData\BPDAction.h"


RBridgePD::RBridgePD()
    : Component()
    , config(Config::GetInstance())
    , configSection("Arm")
//   , state(IDLE)
    , oldState(ACTION::BPD::IDLE)
    , cycleCount(0)
    , presetMode(true)
    , pulseCount(0)
{
    armEsc = new AsyncCANJaguar(RobotConfig::CAN::BPD, "Arm");

#ifdef VIRTUAL
    // arm is ~29 inches
    // speed: 29 in * 1 ft / 12 in * 1.3 rps * 2 pi rad / rev = ~19.7 ft/s
    // ft / turn: 29 in * 1 ft / 12 in * 2 pi rad / rev = ~15.2 ft
    armPot = new VirtualPot(RobotConfig::ANALOG::POT_ARM, 1, 15.2, 19.7);
#else
    armPot = new AnalogChannel(RobotConfig::ANALOG::POT_ARM);
#endif


    // brake when set to 0 to keep the arm in place
    armEsc->ConfigNeutralMode(AsyncCANJaguar::kNeutralMode_Brake);
    Configure();
    printf("Constructed Arm\n");
}

RBridgePD::~RBridgePD()
{
    delete armEsc;
    delete armPot;
}

void RBridgePD::Configure()
{
    minPosition = config->Get<float>(configSection, "minPosition", 280);
    maxPosition = config->Get<float>(configSection, "maxPosition", 530);

    midPositionDeadband = config->Get<float>(configSection, "midPositionDeadband", 10);

    maxPowerUp    = config->Get<float>(configSection, "maxPowerUp", 0.30);
    powerRetainUp = config->Get<float>(configSection, "powerRetainUp", 0.10);
    powerDown     = config->Get<float>(configSection, "powerDown", -0.15);

    midPowerUp    = config->Get<float>(configSection, "midPowerUp", 0.2);
    midPowerDown  = config->Get<float>(configSection, "midPowerDown", -0.15);
    
    pGainDown 	  = config->Get<float>(configSection, "pGainDown", 0.0015); 
    pGainUp	      = config->Get<float>(configSection, "pGainUp", 0.003); 
    pGainMid	  = config->Get<float>(configSection, "pGainMid", 0.01);

    timeoutCycles = (int)(config->Get<int>(configSection, "timeoutMs", 1500) * 1.0 / 1000.0 * 50.0 / 1.0);
}

void RBridgePD::Output()
{

    float potValue = armPot->GetAverageValue();

#ifdef USE_DASHBOARD
    SmartDashboard::Log(potValue, "Arm Pot Value");
#endif

    if(action->master.abort)
    {
        armEsc->SetDutyCycle(0.0);
        action->bridgePD->state = ACTION::BPD::IDLE;
        action->bridgePD->completion_status = ACTION::ABORTED;
        return; // do not allow normal processing (ABORT is not printed...should fix this -dg)
    }

    const bool state_change = (oldState != action->bridgePD->state);
    if(state_change)
        AsyncPrinter::Printf("Arm: %s\n", ACTION::BPD::state_string[action->bridgePD->state]);

    if(state_change)
        cycleCount = timeoutCycles; // reset timeout

    switch(action->bridgePD->state)
    {
    case ACTION::BPD::PRESET_TOP:
        action->bridgePD->completion_status = ACTION::IN_PROGRESS;

        // don't merely switch to the IDLE state, as the caller will likely
        // set the state each time through the loop
        if(--cycleCount < 0)
        {
            action->bridgePD->completion_status = ACTION::FAILURE;
            armEsc->SetDutyCycle(0.0);
            break; // timeout overrides everything
        }

        if(potValue >= maxPosition)
        {
            action->bridgePD->completion_status = ACTION::SUCCESS;
            armEsc->SetDutyCycle(powerRetainUp);
            // cycleCount will never get decremented below 0, so powerRetainUp
            // will be maintained
            cycleCount = 100;
        }
        else //we have not yet hit the setpoint
        {
            action->bridgePD->completion_status = ACTION::IN_PROGRESS;
		    float error = maxPosition - potValue;
            
            armEsc->SetDutyCycle(Util::Max<float>(Util::Min<float>(maxPowerUp*1.5, error*pGainUp), 0.15));

        }
        break;

    case ACTION::BPD::PRESET_BOTTOM:
        action->bridgePD->completion_status = ACTION::IN_PROGRESS;

        if(--cycleCount < 0)
        {
            action->bridgePD->completion_status = ACTION::FAILURE;
            armEsc->SetDutyCycle(0.0);
            break; // timeout overrides everything
        }

        if(potValue <= minPosition)
        {
            action->bridgePD->completion_status = ACTION::SUCCESS;
            armEsc->SetDutyCycle(0.0); // don't go below the min position
            // cycleCount will never get decremented below 0, so powerRetainUp
            // will be maintained
            cycleCount = 100;
        }
        else
        {
            action->bridgePD->completion_status = ACTION::IN_PROGRESS;
            float error = minPosition - potValue;
            armEsc->SetDutyCycle(Util::Max<float>(powerDown, error*pGainDown));
//            AsynchronousPrinter::Printf("setpoint %.3f\n",Util::Max<float>(powerDown, error*0.0015));
        }
        break;
        
    case ACTION::BPD::MANUAL_UP:
        if(potValue < maxPosition)
            armEsc->SetDutyCycle(maxPowerUp);
        else
            armEsc->SetDutyCycle(0.0);

        action->bridgePD->completion_status = ACTION::IN_PROGRESS;
        // operator must hold button to stay in manual mode
        action->bridgePD->state = ACTION::BPD::IDLE;
        break;

    case ACTION::BPD::MANUAL_DOWN:
        if(potValue > minPosition)
            armEsc->SetDutyCycle(powerDown);
        else
            armEsc->SetDutyCycle(0.0);

        action->bridgePD->completion_status = ACTION::IN_PROGRESS;
        // operator must hold button to stay in manual mode
        action->bridgePD->state = ACTION::BPD::IDLE;
        break;

    case ACTION::BPD::IDLE:
        action->bridgePD->completion_status = ACTION::SUCCESS;
        armEsc->SetDutyCycle(0.0);
        break;
    default:
        AsyncPrinter::Printf("Arm: ERROR Unknown State\n");

    }

    oldState = action->bridgePD->state;

    //Print diagnostics
    static ACTION::eCompletionStatus lastDoneState = ACTION::UNSET;
    if(lastDoneState != action->bridgePD->completion_status)
        AsyncPrinter::Printf("Arm: Status=%s\n", ACTION::status_string[action->bridgePD->completion_status]);
    lastDoneState = action->bridgePD->completion_status;
}

string RBridgePD::GetName()
{
    return "Arm";
}