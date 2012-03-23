#include "DigitalModule.h"
#include <WPILib.h>
#include <I2C.h>
#include "../ActionData/ActionData.h"
#include "../ActionData/IMUData.h"
#include "../ActionData/DriveTrainAction.h"
#include "../Util/AsyncPrinter.h"
#include "../Config/RobotConfig.h"
#include "IMU.h"

const UINT8 IMU::kAddress;

IMU::IMU(uint8_t address, uint8_t module_num) :
	AsyncProcess("IMU"), Loggable()
{
	DigitalModule *module = DigitalModule::GetInstance(module_num);
	if (module)
	{
		AsyncPrinter::Printf("Module %d, Addess 0x%x\n", module_num, address);
		// left shift the address to get the required 8-bit form
		m_i2c = module->GetI2C((address << 1));
	}
	m_expected_packet_num = 0;
	m_accel_x = m_accel_y = m_accel_z = 0;
	m_gyro_x = m_gyro_y = m_gyro_z = 0;
	m_roll = m_pitch = m_yaw = 0.0;
	//m_dio = new DigitalOutput(RobotConfig::DIGITAL_IO::IMU_CALIBRATE);
	memset(m_i2c_buf, 0, kNumPackets * 6 + 1);
	m_time = 0;
}

IMU::~IMU()
{
	deinit();
	delete m_i2c;
	//delete m_dio;
	m_i2c = NULL;
}

void IMU::work()
{
	update(ActionData::GetInstance());
	static int lastUpdate = 0;
	static int counter = 0;
	counter++;
	if (GetFPGATime() - lastUpdate > 1000000)
	{
		AsyncPrinter::Printf("%d packets in 1s\n", counter);
		counter = 0;
		lastUpdate = GetFPGATime();
	}
}

void IMU::update()
{
	m_time = GetFPGATime();
	if (getPacket())
	{
		return;
	}
	m_status = getUint8(STATUS);
	if (m_status == 0xff)
	{
		AsyncPrinter::Printf("Status: Bad IMU packet\r\n");
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

	m_gyro_y_delta = getGyroY() - m_last_gyro_y;
	m_last_gyro_y = getGyroY();

	m_time = GetFPGATime() - m_time;
}

void IMU::update(ActionData * action)
{
	update();
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

	DriveTrainAction * dt = action->drivetrain;

	if (dt->rate.desiredDriveRate == 0.0 && dt->rate.desiredTurnRate == 0.0
			&& dt->robotRotateSpeed == 0 && dt->robotTranslateSpeed == 0)
	{
		//m_dio->Set(1);
	}
	else
	{
		//m_dio->Set(0);
	}
}

void IMU::log()
{
#if LOGGING_ENABLED
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

	sdb->PutDouble("IMU Gyro y delta", getGyroYDelta());
#endif
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
			AsyncPrinter::Printf("I2C error status=%d offset=%d\r\n",
					readstatus, data[0]);
			return -1;
		}
		else if (data[0] == m_expected_packet_num)
		{
			m_expected_packet_num = (m_expected_packet_num + 1) % kNumPackets;
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

double IMU::getGyroYDelta()
{
	return m_gyro_y_delta;
}

double IMU::getGyroZ()
{
	return m_gyro_z * kGyroConversion;
}

void IMU::printAll()
{
	AsyncPrinter::Printf("IMU Data: \r\n");
	AsyncPrinter::Printf("Update count %d\n", m_status);
	AsyncPrinter::Printf("Roll: %2f Pitch: %2f Yaw: %2f\r\n", getRoll(),
			getPitch(), getYaw());
	AsyncPrinter::Printf("Scaled Gyro: [%2f, %2f, %2f]\r\n", getGyroX(),
			getGyroY(), getGyroZ());
	AsyncPrinter::Printf("Scaled Accel: [%2f, %2f, %2f]\r\n", getAccelX(),
			getAccelY(), getAccelZ());
	AsyncPrinter::Printf("\r\n");
}

