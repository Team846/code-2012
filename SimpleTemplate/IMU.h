/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __ADXL345_2__I2C_h__
#define __ADXL345_2__I2C_h__

#include "SensorBase.h"

class I2C;

class IMU: public SensorBase
{
protected:
	const static UINT8 kAddress = (0x29) << 1/* >> 1*/;
	const static uint8_t kNumPackets = 4;
	//	static const UINT8 kAddress = (0x28);

public:
	explicit IMU();
	virtual ~IMU();
	void update();
	int getPacket();

	double getRoll();
	double getYaw();
	double getPitch();

	int16_t getAccelX();
	int16_t getAccelY();
	int16_t getAccelZ();

	int16_t getGyroX();
	int16_t getGyroY();
	int16_t getGyroZ();
	
	void printAll();

protected:
	I2C* m_i2c;

private:
	enum DATA_STRUCT
	{
		STATUS = 0x00,
		PITCH = 0x01,
		ROLL = 0x03,
		YAW = 0x05,
		ACCEL_X = 0x07,
		ACCEL_Y = 0x09,
		ACCEL_Z = 0x0b,
		GYRO_X = 0x0d,
		GYRO_Y = 0x0f,
		GYRO_Z = 0x11,
	};

	uint8_t m_i2c_buf[kNumPackets * 6 + 1];
	int16_t getInt16(uint8_t index);
	uint8_t getUint8(uint8_t index);
	uint8_t PacketNumberExpected;
	int16_t m_accel_x, m_accel_y, m_accel_z, m_gyro_x, m_gyro_y, m_gyro_z;
	double m_roll, m_pitch, m_yaw;
};

#endif

