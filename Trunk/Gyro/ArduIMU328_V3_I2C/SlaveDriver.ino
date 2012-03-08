/*

Modified:       Alex Kanaris
Date:           2-25-2012

Revision Log:

2-25-2012
Split up the IMU data message to 7-byte chunks to accomodate I2C master device driver limistation
The 7-byte message consists of 1-byte message prefix plus 6-bytes payload. 
Changed Blue LED from always ON in normal operation to be ON during I2C slave access, OFF otherwise
Added Red LED ON during I2C slave write accesses

*/

#include "SlaveDriver.h"
#include "MPU6000.h"


void I2C_Slave_Init(void)
{
  if(useInterrupt)
  {
    pinMode(INTERRUPT_PIN,OUTPUT);
    digitalWrite(INTERRUPT_PIN,HIGH);
  }
  Wire.begin(SLAVE_ADDRESS); 
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  reg.Ident = regtemp.Ident = IDENTIFICATION;
  reg.Revision = regtemp.Revision = REVISION;
  
  regtemp.Status = 1;
}




void changeModeConfig()
{
  /*Put your code here to evaluate which of the registers need changing
   And how to make the changes to the given device.  For our GPS example
   It could be issuing the commands to change the baud rate, update rate,
   Datum, etc…
   We just put some basic code below to copy the data straight to the registers*/
   if(zeroB)
      {
        regtemp.Mode = modeRegister = zeroBData;
        zeroB = 0;  //always make sure to reset the flags before returning from the function
        zeroBData = 0;
      }
      if(zeroC)
      {
        regtemp.Config = configRegister = zeroCData;
        zeroC = 0;  //always make sure to reset the flags before returning from the function
        zeroCData = 0;
      }
}




void storeData()
{
 
  regtemp.roll = (int)(ToDeg(MPU6000.MPU_roll)*100);
  regtemp.pitch = (int)(ToDeg(MPU6000.MPU_pitch)*100);
  regtemp.yaw = (int)(ToDeg(MPU6000.MPU_yaw)*100);
  
  regtemp.accelX = MPU6000.accelX;
  regtemp.accelY = MPU6000.accelY;
  regtemp.accelZ = MPU6000.accelZ;
  
  regtemp.gyroX = MPU6000.gyroX;
  regtemp.gyroY = MPU6000.gyroY;
  regtemp.gyroZ = MPU6000.gyroZ;
  
  
  regtemp.magnetomX = magnetom_x;
  regtemp.magnetomY = magnetom_y;
  regtemp.magnetomZ = magnetom_z;
  
  newDataAvailable++;  // Keep track of how many times new data have updated since last time read
  regtemp.Status = newDataAvailable;
 
}

byte m[8];

void requestEvent()
{
  //bitSet(PINB, 0);  // Toggle Pin D8
  digitalWrite(BLUE,HIGH);
#if 0 // PRINT
  Serial.print("Req. Event ");
  Serial.print(m[0],DEC);
  Serial.print(" \t ");
  Serial.print(newDataAvailable,DEC);
#endif
  byte *ptr;
  if(newDataAvailable && m[0] == 0)
  {
    reg = regtemp;  
  }
  ptr = (byte *) &reg;
  
  for (uint8_t i = 0; i < 6; i++) {
    m[i+1] =  ptr[m[0] * 6 + i];
  }
  Wire.write(m, 7);
  m[0] = ++m[0] % 4;
   
  if (m[0] == 0) 
  {
   reg.Status = 0;  //Reset status to 0 once new data is sent.  Will only be reset to 1 when new data becmes available
    newDataAvailable = 0;
  }
   //bitSet(PINB, 0);  // Timing measured on 1-29-2012 delta T = 35 uS
#if 0 //PRINT
  Serial.println(" I2C");
#endif
  digitalWrite(BLUE,LOW);
}



void receiveEvent(int bytesReceived)
{
  digitalWrite(RED,HIGH);
  for (int a = 0; a < bytesReceived; a++)
  {
    if ( a < MAX_SENT_BYTES)
    {
      receivedCommands[a] = Wire.read();
#if PRINT
      Serial.println("Received valid command");
#endif
    }
    else
    {
      Wire.read();  // if we receive more data then allowed just throw it away
#if PRINT
      Serial.println("Received too much data. Ignored");
#endif
    }
  }
  if(bytesReceived == 1 && (receivedCommands[0] < REG_MAP_SIZE))
  {
#if PRINT
    Serial.println("Received too much data. Ignored");
#endif
    digitalWrite(RED,LOW);
    return; 
  }
  if(bytesReceived == 1 && (receivedCommands[0] >= REG_MAP_SIZE))
  {
#if PRINT
    Serial.println("Received command 0");
#endif
    receivedCommands[0] = 0x00;
    digitalWrite(RED,LOW);
    return;
  }
  switch(receivedCommands[0]){
  case 0x19:
    zeroB = 1; //this variable is a status flag to let us know we have new data in register 0x0B
    zeroBData = receivedCommands[1]; //save the data to a separate variable
    bytesReceived--;
    if(bytesReceived == 1)  //only two bytes total were sent so we’re done
    {
      digitalWrite(RED,LOW);
      return;
    }
    zeroC = 1;
    zeroCData = receivedCommands[2];
    digitalWrite(RED,LOW);
    return; //we can return here because the most bytes we can receive is three anyway
    break;
  case 0x1A:
    zeroC = 1;
    zeroCData = receivedCommands[1];
    digitalWrite(RED,LOW);
    return; //we can return here because 0x0C is the last writable register
    break;
  default:
    digitalWrite(RED,LOW);
    return; // ignore the commands and return
  }
}

