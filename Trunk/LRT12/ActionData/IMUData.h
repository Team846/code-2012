#ifndef IMU_DATA_H_
#define IMU_DATA_H_

struct IMU_Data
{
	double roll, pitch, yaw;
	int16_t accel_x, accel_y, accel_z;
	int16_t gyro_x, gyro_y, gyro_z;
};

#endif
