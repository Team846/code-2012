/*

Author:         David Wachenschwanz 
Date:           1-29-2012
Modified:       Alex Kanaris
Date:           2-24-2012

Revision Log:

1-29-2012
Setup ATMEGA communication to host from I2C master to I2C slave

2-24-2012
Disabled magnetometer I2C access to
Bypassed magnetometer based yaw drift cancellation
Changed Blue LED from always ON in normal operation to be ON during I2C slave access, OFF otherwise


For use by Lynbrook High School Robotics Funky Monkees, FRC Team #846

-  MPU6000 and Compass code based on code from Jordi Munoz, Jose Julio, Pat Hickey, www.diydrones.com
-  I2C slave code based on code from Wayne Truchsess, 


This firmware is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

*/

#include <SPI.h>
#include <Wire.h>
#include "MPU6000.h" // APM 2.0 MPU6000 Library
#include "SlaveDriver.h"  // I2C Slave Library

#define PRINT  1
#define MAGNETO 0

#define ToDeg(x) (x*57.2957795131)  // radians to degrees :  *180/pi
#define RED 5
#define BLUE 6

MPU6000_Class MPU6000;

int magnetom_x;
int magnetom_y;
int magnetom_z;
float MAG_Heading;

void Print_euler_angles()
{
  
  Serial.print ("\tPitch:  ");
  Serial.print(ToDeg(MPU6000.MPU_pitch));
  Serial.print("\tRoll:  ");
  Serial.print(ToDeg(MPU6000.MPU_roll));
  Serial.print ("\tYaw:  ");
  Serial.print(ToDeg(MPU6000.MPU_yaw));
  Serial.print("\t");
}


void setup()
{
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  digitalWrite(RED,LOW);
  digitalWrite(BLUE,LOW);
  
  bitSet(DDRB, 0);  // Set pin D8 as output for toggling interrupt when new data available
  
 magnetom_x = 0;
 magnetom_y = 0;
 magnetom_z = 0;
 MAG_Heading = 0;
  
  
  #if PRINT
    Serial.begin(115200);
    Serial.println("MonkeyPilot FIFO");
  #endif
   
  MPU6000.init();
#if MAGNETO
  Compass_Init();
#endif
  I2C_Slave_Init();

  digitalWrite(RED,HIGH);
  
  #if PRINT  
    Serial.println("Calibrating gyros and accels (keep board flat and still)...");
  #endif
  
  MPU6000.gyro_offset_calibration();
  MPU6000.accel_offset_calibration();
  
  #if PRINT
    Serial.println("Calibration done.");
  #endif
  
  digitalWrite(RED,LOW);
  
  
  // Initialize the MPU6000 DMP 
  MPU6000.dmp_init();
  MPU6000.set_dmp_rate(MPU6000_50HZ);  // set the dmp output rate to 50Hz
  MPU6000.read_FIFO();
  MPU6000.FIFO_reset();
  
  digitalWrite(BLUE,LOW);
}


void loop()
{
 
  if(MPU6000.newdata())
  { 
       
       //bitSet(PINB, 0);
       if(MPU6000.FIFO_ready())
       {
           MPU6000.FIFO_getPacket();
           MPU6000.read_FIFO();
           MPU6000.calculate();        // calculate euler angles solution
          // we run the gyro bias correction from gravity vector algorithm
          MPU6000.gyro_bias_correction_from_gravity();
       }
       //bitSet(PINB, 0);  // Timing Measured on 1-29-2012 delta T = 1.2 mS
#if MAGNETO
       Read_Compass();
       
       MAG_Heading = MPU6000.compass_angle(magnetom_x, magnetom_y, magnetom_z, 14.2);
       MPU6000.update_yaw_compass_correction(MAG_Heading);
       MPU6000.get_yaw_compass_corrected();
#endif

       /*
       //Compass_Heading();
       Serial.print("Heading:  ");
       Serial.print(MAG_Heading);
       Serial.print("\t");
       */
       
  
        //bitSet(PINB, 0);  // Timing Measured on 1-29-2012 delta T = 1.1 mS
        storeData();
        bitSet(PINB, 0);  // Timing Measured on 1-29-2012 delta T = 80 uS
        delay(1);
        bitSet(PINB, 0);
   
#if 0 //DON'T WASTE CYCLES PRINTING
        Print_euler_angles();
   
       
        Serial.print("Accels: ");
        Serial.print(MPU6000.accelX);
        Serial.print ("\t");
        Serial.print(MPU6000.accelY);
        Serial.print ("\t");
        Serial.print(MPU6000.accelZ);
        Serial.print ("\t");
        
        Serial.print("Gyros: ");
        Serial.print(MPU6000.gyroX);
        Serial.print ("\t");
        Serial.print(MPU6000.gyroY);
        Serial.print ("\t");
        Serial.print(MPU6000.gyroZ);
        Serial.print ("\t");
        
        Serial.print("Mag: ");
        Serial.print(magnetom_x);
        Serial.print ("\t");
        Serial.print(magnetom_y);
        Serial.print ("\t");
        Serial.print(magnetom_z);
        Serial.print ("\t");
        
        Serial.println();
#endif
        
  }   
/*  
 if(zeroB||zeroC)
   changeModeConfig();
  */ 
 
}

