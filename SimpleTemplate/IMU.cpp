
#include "DigitalModule.h"
#include <WPILib.h>
#include "IMU.h"
#include <I2C.h>
#include <taskLib.h>

const UINT8 IMU::kAddress;

/**
 * Constructor.
 * 
 * @param moduleNumber The digital module that the sensor is plugged into (1 or 2).
 * @param range The range (+ or -) that the accelerometer will measure.
 */
IMU::IMU()
{
	int moduleNumber = 1;
	DigitalModule *module = DigitalModule::GetInstance(moduleNumber);
	if (module)
	{
		printf("Module %d, Addess %d\n", moduleNumber, kAddress);
		m_i2c = module->GetI2C(kAddress);
	}
	m_accel_x = m_accel_y = m_accel_z = 0;
	m_gyro_x = m_gyro_y = m_gyro_z = 0;
	m_roll = m_pitch = m_yaw = 0.0;
}

/**
 * Destructor.
 */
IMU::~IMU()
{
	delete m_i2c;
	m_i2c = NULL;
}

void IMU::update()
{
	if (getPacket())
	{
		return;
	}
	uint8_t status = getUint8(STATUS);
	if (status == 0x00 || status == 0xff)
	{
		printf("Error bad IMU packet\r\n");
		return;
	}
	m_pitch = getInt16(PITCH) / 100.0;
	m_roll = getInt16(ROLL) / 100.0;
	m_yaw = getInt16(YAW) / 100.0;

	m_accel_x = getInt16(ACCEL_X);
	m_accel_y = getInt16(ACCEL_Y);
	m_accel_z = getInt16(ACCEL_Z);

	m_gyro_x = getInt16(GYRO_X);
	m_gyro_y = getInt16(GYRO_Y);
	m_gyro_z = getInt16(GYRO_Z);
}

int IMU::getPacket()
{
	for (uint8_t i = 0; i < kNumPackets; ++i)
	{
		uint8_t data[8];
		memset(data, 0, 7);
		bool readstatus = m_i2c->Transaction(NULL, 0, data, 7);
		if (data[0] >= kNumPackets || readstatus)
		{
			printf("I2C error status=%d offset=%d\r\n", readstatus, data[0]);
			return -1;
		}
		else
		{
			for (uint8_t j = 0; j < 6; ++j)
			{
				m_i2c_buf[data[0] * 6 + j] = data[j + 1];
			}
		}
	}
	printf("Packet: ");
	for (uint8_t i = 0; i < kNumPackets * 6; i++)
	{
		printf("%d ", m_i2c_buf[i]);
	}
	printf("\r\n");
	return 0;
}

int16_t IMU::getInt16(uint8_t index)
{
	return (int16_t) (m_i2c_buf[index] << 8) | (m_i2c_buf[index + 1]);
}

uint8_t IMU::getUint8(uint8_t index)
{
	return m_i2c_buf[index];
}

double IMU::getRoll()
{
	return m_roll;
}

double IMU::getYaw()
{
	return m_yaw;
}

double IMU::getPitch()
{
	m_pitch;
}

int16_t IMU::getAccelX()
{
	return m_accel_x;
}

int16_t IMU::getAccelY()
{
	return m_accel_y;
}

int16_t IMU::getAccelZ()
{
	return m_accel_z;
}

int16_t IMU::getGyroX()
{
	return m_gyro_x;
}

int16_t IMU::getGyroY()
{
	return m_gyro_y;
}

int16_t IMU::getGyroZ()
{
	return m_gyro_z;
}

void IMU::printAll()
{
	printf("IMU Data: \r\n");
	printf("Roll: %.02f Pitch: %.02f Yaw: %.02f\r\n", m_roll, m_pitch, m_yaw);
	printf("Gyro: [%d, %d, %d]\r\n", m_gyro_x, m_gyro_y, m_gyro_z);
	printf("Accel: [%d, %d, %d]\r\n", m_accel_x, m_accel_y, m_accel_z);
	printf("\r\n");
}
