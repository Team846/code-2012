#ifndef IMU_DATA_H_
#define IMU_DATA_H_

struct IMU_Data
{
	double roll, pitch, yaw;
	double accel_x, accel_y, accel_z;
	double gyro_x, gyro_y, gyro_z;
	IMU_Data()
	{
		roll = pitch = yaw = accel_x = accel_y = accel_z = gyro_x = gyro_y
				= gyro_z = 0;
	}
};

#endif
