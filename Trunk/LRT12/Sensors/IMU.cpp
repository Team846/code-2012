#include "DigitalModule.h"
#include <WPILib.h>
#include <I2C.h>
#include "../ActionData/ActionData.h"
#include "../ActionData/IMUData.h"
#include "IMU.h"

const UINT8 IMU::kAddress;

IMU::IMU(uint8_t address, uint8_t module_num) :
	Loggable()
{
	DigitalModule *module = DigitalModule::GetInstance(module_num);
	if (module)
	{
		printf("Module %d, Addess 0x%x\n", module_num, address);
		// left shift the address to get the required 8-bit form
		m_i2c = module->GetI2C((address << 1));
	}
	m_expected_packet_num = 0;
	m_accel_x = m_accel_y = m_accel_z = 0;
	m_gyro_x = m_gyro_y = m_gyro_z = 0;
	m_roll = m_pitch = m_yaw = 0.0;
}

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
		printf("Status: Bad IMU packet\r\n");
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

void IMU::update(ActionData * action)
{
	update(); // update I2C data
	IMU_Data * imu = action->imu;

	imu->roll = getRoll();
	imu->pitch = getPitch();
	imu->yaw = getYaw();

	imu->accel_x = getAccelX();
	imu->accel_y = getAccelY();
	imu->accel_z = getAccelZ();

	imu->gyro_x = getGyroX();
	imu->gyro_y = getGyroY();
	imu->gyro_z = getGyroZ();
}

void IMU::log()
{
	SmartDashboard * sdb = SmartDashboard::GetInstance();

	sdb->PutDouble("IMU Roll", getRoll());
	sdb->PutDouble("IMU Pitch", getPitch());
	sdb->PutDouble("IMU Yaw", getYaw());

	sdb->PutDouble("IMU Accel X", getAccelX());
	sdb->PutDouble("IMU Accel Y", getAccelY());
	sdb->PutDouble("IMU Accel Z", getAccelZ());

	sdb->PutDouble("IMU Gyro X", getGyroX());
	sdb->PutDouble("IMU Gyro Y", getGyroY());
	sdb->PutDouble("IMU Gyro Z", getGyroZ());
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
		else if (data[0] == m_expected_packet_num)
		{
			m_expected_packet_num = ++m_expected_packet_num % kNumPackets;
			for (uint8_t j = 0; j < 6; ++j)
			{
				m_i2c_buf[data[0] * 6 + j] = data[j + 1];
			}
		}
		else
		{
			m_expected_packet_num = 0;
			return 1;
		}
	}
	return 0;
}

int16_t IMU::getInt16(uint8_t index)
{
	return (int16_t) (m_i2c_buf[index + 1] << 8) | (m_i2c_buf[index]);
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
	return m_pitch;
}

double IMU::getAccelX()
{
	return m_accel_x * kAccelConversion;
}

double IMU::getAccelY()
{
	return m_accel_y * kAccelConversion;
}

double IMU::getAccelZ()
{
	return m_accel_z * kAccelConversion;
}

double IMU::getGyroX()
{
	return m_gyro_x * kGyroConversion;
}

double IMU::getGyroY()
{
	return m_gyro_y * kGyroConversion;
}

double IMU::getGyroZ()
{
	return m_gyro_z * kGyroConversion;
}

void IMU::printAll()
{
	printf("IMU Data: \r\n");
	printf("Roll: %.02f Pitch: %.02f Yaw: %.02f\r\n", m_roll, m_pitch, m_yaw);
	printf("Gyro: [%d, %d, %d]\r\n", m_gyro_x, m_gyro_y, m_gyro_z);
	printf("Accel: [%d, %d, %d]\r\n", m_accel_x, m_accel_y, m_accel_z);
	printf("Scaled AcceL: [%.02f, %.02f, %.02f]\r\n", getAccelX(), getAccelY(),
			getAccelZ());
	printf("\r\n");
}

