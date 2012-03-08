#ifndef __SLAVEDRIVER_H__
#define __SLAVEDRIVER_H__

#include <arduino.h>
#include <Wire.h>


#define  SLAVE_ADDRESS           0x29  //slave address,any number from 0x01 to 0x7F
#define  REG_MAP_SIZE            0x1D
#define  MAX_SENT_BYTES          3
#define  INTERRUPT_PIN           8
#define  IDENTIFICATION          0x0F
#define  REVISION                0x01


struct RegMap{
  byte Status;
  int pitch;
  int roll;
  int yaw;
  int accelX;
  int accelY;
  int accelZ;
  int gyroX;
  int gyroY;
  int gyroZ;
  int magnetomX;
  int magnetomY;
  int magnetomZ;
  byte Mode;
  byte Config;
  byte Ident;
  byte Revision;
}__attribute__((__packed__));  //NOTE:  __packed__ attribute forces struct to align to 1-byte boundaries.


RegMap reg;
RegMap regtemp;

/********* Global  Variables  ***********/
//byte registerMap[REG_MAP_SIZE];
//byte registerMapTemp[REG_MAP_SIZE];
byte receivedCommands[MAX_SENT_BYTES];
byte newDataAvailable = 0;
byte useInterrupt = 1;
byte modeRegister = 0;
byte configRegister = 0;
byte zeroB = 0;
byte zeroC = 0;
byte zeroBData = 0;
byte zeroCData = 0;


void I2C_Slave_Init(void);


#endif
