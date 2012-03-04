#ifndef IMU_H_
#define IMU_H_

#include "../Log/Loggable.h"

class I2C;
class ActionData;

/*!
 * I2C inteface with Arduimu, running specific code on the IMU side.
 * @author Robert Ying
 * @author Brian Axelrod
 * @author Alexander Kanaris
 */
class IMU: public Loggable
{
public:
	/*!
	 * Constructs an IMU object and initializes the I2C
	 * @param address I2C address of the slave, defaults to 0x29. 7-bit.
	 * @param module digital module to init I2C on, defaults to 1
	 */
	explicit IMU(uint8_t address = kAddress, uint8_t module = 1);

	/*!
	 * Destroys an IMU object and deletes the I2C handle
	 */
	virtual ~IMU();

	/*!
	 * To be called before any get() methods, such that data is fresh. blocking.
	 */
	void update();

	/*!
	 * Updates actiondata directly
	 * @param action pointer to actiondata
	 */
	void update(ActionData * action);

	/*!
	 * Returns most recent roll value
	 * @return roll, in degrees
	 */
	double getRoll();

	/*!
	 * Returns most recent yaw value
	 * @return yaw, in degrees
	 */
	double getYaw();

	/*!
	 * Returns most recent pitch value
	 * @return pitch, in degrees
	 */
	double getPitch();

	/*!
	 * Returns raw acceleration in x in m/s^2
	 * @return acceleration in x
	 */
	double getAccelX();

	/*!
	 * Returns raw acceleration in y in m/s^2
	 * @return acceleration in y
	 */
	double getAccelY();

	/*!
	 * Returns raw acceleration in z in m/s^2
	 * @return acceleration in z
	 */
	double getAccelZ();

	/*!
	 * Returns raw angular rate about x in degrees per second
	 * @return angular rate about x
	 */
	double getGyroX();

	/*!
	 * Returns raw angular rate about y in degrees per second
	 * @return angular rate about y
	 */
	double getGyroY();

	/*!
	 * Returns raw angular rate about z in degrees per second
	 * @return angular rate about z
	 */
	double getGyroZ();

	/*!
	 * Prints all data via printf for debug
	 */
	void printAll();

	/*!
	 * Logs data
	 */
	virtual void log();

	/*!
	 * Starts the task
	 */
	void releaseSemaphore();
	
	/*!
	 * 
	 */
	void startTask();
	
	/*!
	 * 
	 */
	void stopTask();
	
private:
	/*!
	 * Helper method to get kNumPackets of data and fill m_i2c_buf
	 * @return 0 on success
	 */
	int getPacket();

	/*!
	 * starts the task
	 * @param ptr to the IMU
	 */
	static void taskEntryPoint(int ptr);

	/*!
	 * Is the task
	 */
	void task();

	const static uint8_t kAddress = (0x29); // 7-bit default address
	const static uint8_t kNumPackets = 4; // number of 7-byte packets to concatenate
	const static double kAccelConversion = 0.001/*9.81 / 8192*/; // 4G, 4096 = 1G
	const static double kGyroConversion = 0.001/*0.94*/; // 3.33mV/deg

	/*!
	 * indices in the data packet of various variables
	 */
	enum DATA_STRUCT
	{
		STATUS = 0x00, //!< STATUS number of updates on IMU since last request
		PITCH = 0x01, //!< PITCH fixed-point pitch in degrees * 100
		ROLL = 0x03, //!< ROLL fixed-point roll in degrees * 100
		YAW = 0x05, //!< YAW fixed-point yaw in degrees * 100
		ACCEL_X = 0x07, //!< ACCEL_X raw acceleration in x
		ACCEL_Y = 0x09, //!< ACCEL_Y raw acceleration in y
		ACCEL_Z = 0x0b, //!< ACCEL_Z raw acceleration in z
		GYRO_X = 0x0d, //!< GYRO_X raw angular velocity about x
		GYRO_Y = 0x0f, //!< GYRO_Y raw angular velocity about y
		GYRO_Z = 0x11,
	//!< GYRO_Z raw angular velocity about z
	};

	I2C* m_i2c;
	uint8_t m_i2c_buf[kNumPackets * 6 + 1];
	int16_t getInt16(uint8_t index);
	uint8_t getUint8(uint8_t index);
	uint8_t m_expected_packet_num;
	int16_t m_accel_x, m_accel_y, m_accel_z, m_gyro_x, m_gyro_y, m_gyro_z;
	double m_roll, m_pitch, m_yaw;
	int m_time;

	SEM_ID m_sem;
	Task *m_task;
	bool m_is_running;
};

#endif

