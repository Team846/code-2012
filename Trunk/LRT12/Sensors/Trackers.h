#ifndef TRACKERS_H_
#define TRACKERS_H_

#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

//#include <netinet/in.h> // <-- DO NOT USE
//#include <arpa/inet.h> // <-- BLACK MAGIC

#include <iostream>

#include <WPILib.h>
#include "..\Config\Configurable.h"
#include "../Log/Loggable.h"
#include "../Util/AsyncPrinter.h"
#include "../ActionData/ActionData.h"
#include "../ActionData/CameraData.h"

/*!
 * @brief Receives data from the key and target trackers.
 * 
 * @author Tony Peng
 */

class Trackers: public Loggable, public Configurable
{
public:
	/*!
	 * @brief values to get 
	 */
	enum KeyValue
	{
		HIGHER, LOWER, RED, BLUE
	};

	/*! 
	 * @brief Creates a new instance of Trackers 
	 * @param  task_priority The task priority to assign to the listen task.
	 */
	Trackers(int task_priority = Task::kDefaultPriority);
	/*! 
	 * @brief Destructs the current instance. 
	 */
	~Trackers();

	/*!
	 * @brief Listen function that the task calls. 
	 */
	static int listenTask(uint32_t obj);

	/*!
	 * @brief Begins listening for incoming packets on UDP 8000.
	 */
	void listen();

	/*! 
	 * @brief Stops the current listening operation.
	 * @param force Determines whether or not the task should forcefully be stopped. 
	 */
	void stop(bool force = false);

	/*! 
	 * @brief Gets either the red or blue key value. 
	 * @param v Determines whether or not the red or the blue value should be returned 
	 * */
	uint8_t getKeyValue(KeyValue v);

	/*! 
	 * @brief Gets the target offset from side of frame 
	 */
	uint32_t getTargetOffset();

	/*! 
	 * @brief Gets the target distance
	 */
	uint32_t getTargetDistance();

	/*! 
	 * @brief Gets the target that is being tracked
	 */
	bool getSelectedTarget();

	/*!
	 * logs data
	 */
	virtual void log();
	
	/*! 
	 * Loads the values for targetting
	 */
	virtual void Configure();

private:

	/*! 
	 * @brief Sets up the socket. 
	 */
	int setup();

	/*! 
	 * @brief Disconnects and closes the socket. 
	 */
	void disconnect();

	/*!
	 * Updates actiondata
	 */
	void update();

	Task *m_task;

	int m_socket;
	struct sockaddr_in m_server_address;
	struct sockaddr_in m_client_address;
	int m_address_length;
	char m_input_buffer[200];

	bool m_task_is_done;
	bool m_is_running;

	uint8_t m_key_value_r;
	uint8_t m_key_value_b;

	int8_t m_target_slop; 
	uint32_t m_target_x;
	uint32_t m_align_setpoint;
	uint32_t m_align_threshold;
	
	uint32_t m_target_dist;
	bool m_target_top;

	int m_key_missed_packets;
	int m_target_missed_packets;
	
	DigitalOutput m_indicator_r;
	DigitalOutput m_indicator_g;
	DigitalOutput m_indicator_b;
		
	
	typedef struct {
		uint32_t x, y, distance;
	} TargetInfo;

	DISALLOW_COPY_AND_ASSIGN(Trackers);
};
#endif // TRACKERS_H_
