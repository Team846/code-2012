#ifndef ROBOT_CONFIG_H_
#define ROBOT_CONFIG_H_

#define CHANGEME 0

namespace RobotConfig
{
const uint32_t INVALID = ~0; //use to mark unused ports below. Will cause errors. -dg
namespace CAN //ports 1-16 (2CAN limitation)
{
const uint32_t DRIVE_RIGHT_A = CHANGEME;
const uint32_t DRIVE_RIGHT_B = CHANGEME;
const uint32_t DRIVE_LEFT_A = CHANGEME;
const uint32_t DRIVE_LEFT_B = CHANGEME;
const uint32_t ARM_ = CHANGEME; //name "ARM" seems to be used -dg
const uint32_t ROLLER_BOTTOM = CHANGEME;
const uint32_t ROLLER_TOP = CHANGEME;
const uint32_t DEPLOYER = CHANGEME;
const uint32_t LIFT = CHANGEME;
}

namespace PWM //ports 1-10
{
const uint32_t RIGHT_GEARBOX_SERVO = CHANGEME;
const uint32_t LEFT_GEARBOX_SERVO = CHANGEME;
const uint32_t ALIGNER_SERVO = CHANGEME;
}

namespace ANALOG //ports 1-7; 8 is used by FRC's 12V battery monitor
{
const uint32_t POT_ARM = CHANGEME;
const uint32_t LINE_SENSOR_ADC = CHANGEME;
}

namespace DIGITAL_IO //ports 1-14
{
const uint32_t LINE_SENSOR_SI = CHANGEME;
const uint32_t LINE_SENSOR_CLOCK = CHANGEME;
const uint32_t ENCODER_LEFT_A = CHANGEME;
const uint32_t ENCODER_LEFT_B = CHANGEME;
const uint32_t ENCODER_RIGHT_A = CHANGEME;
const uint32_t ENCODER_RIGHT_B = CHANGEME;
}

}
#endif