#ifndef ROBOT_CONFIG_H_
#define ROBOT_CONFIG_H_

#define CHANGEME 0

namespace RobotConfig
{
const uint32_t INVALID = ~0; //use to mark unused ports below. Will cause errors. -dg
const uint32_t LOOP_RATE = 50; //hertz

namespace CAN //ports 1-16 (2CAN limitation)
{
const uint32_t DRIVE_LEFT_A = 2;
const uint32_t DRIVE_LEFT_B = 3;
const uint32_t DRIVE_RIGHT_A = 4;
const uint32_t DRIVE_RIGHT_B = 5;
const uint32_t FEEDER_FRONT = 6;
const uint32_t COLLECTOR = 7;
const uint32_t LAUNCHER = 8;
const uint32_t FEEDER_BACK = 9;
const uint32_t FRONT_INTAKE = 10;
}

namespace PWM //ports 1-10
{
const uint32_t RIGHT_GEARBOX_SERVO = 1;
const uint32_t LEFT_GEARBOX_SERVO = 2;
}

namespace ANALOG //ports 1-7; 8 is used by FRC's 12V battery monitor
{
const uint32_t POT_ARM = CHANGEME;
}

namespace DIGITAL_IO //ports 1-14
{
const uint32_t ENCODER_LEFT_A = 4;
const uint32_t ENCODER_LEFT_B = 3;
const uint32_t ENCODER_RIGHT_A = 1;
const uint32_t ENCODER_RIGHT_B = 2;
const uint32_t COMPRESSOR_PRESSURE_SENSOR_PIN = 14;
const uint32_t HALL_EFFECT = 13;
const uint32_t WEDGE_LIMIT_TOP = CHANGEME;
const uint32_t WEDGE_LIMIT_BOTTOM = CHANGEME;
}

namespace SOLENOID_IO // ports 1-8
{
const uint32_t BALL_COLLECTOR_A = 1;
const uint32_t BALL_COLLECTOR_B = 2;
const uint32_t SHOOTER_SELECT_A = 3;
const uint32_t SHOOTER_SELECT_B = 4;
const uint32_t WEDGE_LATCH = 6;
const uint32_t SHIFTER_A = 7;
const uint32_t SHIFTER_B = 8;
}

namespace RELAY_IO // ports 1-8
{
const uint32_t COMPRESSOR_RELAY = 1;
const uint32_t PRESSURE_PLATE = 4;
const uint32_t WEDGE_SPIKE = 8;
}

namespace DRIVER_STATION
{
const uint32_t COMPRESSOR = 1;
const uint32_t DRIVETRAIN = 2;
const uint32_t SHIFTER = 3;
const uint32_t LAUNCHER = 4;
const uint32_t BALLCOLLECTOR = 5;
const uint32_t WEDGE = 6;
const uint32_t BALLFEEDER = 7;
}

}
#endif
