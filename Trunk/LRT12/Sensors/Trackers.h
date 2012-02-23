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
#include "../Util/AsyncPrinter.h"

using namespace std;

/*!
 * @brief Receives data from the key and target trackers.
 * 
 * @author Tony Peng
 */

class Trackers
{
public:
	/*! @brief Creates a new instance of Trackers 
	 *  @param  task_priority The task priority to assign to the listen task. */
	Trackers(int task_priority=Task::kDefaultPriority);
	/*! @brief Destructs the current instance. */
	~Trackers();

	/*! @brief Listen function that the task calls. */
	static int listenTask(uint32_t obj);
	
	/*! @brief Begins listening for incoming packets on UDP 8000. */
	void listen();
	/*! @brief Stops the current listening operation.
	 *  @param force Determines whether or not the task should forcefully be stopped. */
	void stop(bool force=false);
	
	/*! @brief Gets either the red or blue key value. 
	 *  @param trueisredfalseisblue Determines whether or not the red or the blue value should be returned (true is red, blue is false). */
	uint8_t getKeyValue(bool trueisredfalseisblue); // <-- PERHAPS THE LONGEST VARIABLE IN THE CODEBASE
	
	/* TODO: Ask Eva what "slop" and "top" mean... */
	/*! @brief Gets the target slop value */
	uint8_t getTargetSlop();
	/*! @brief Gets the top status of the target. */
	bool getTargetTop();
	
private:
	
	/*! @brief Sets up the socket. */
	int setup();
	/*! @brief Disconnects and closes the socket. */
	void disconnect();
	
	Task *m_task;
	
	int m_socket;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int addr_len;
	char in_buf[7];
	
	bool taskDone;
	bool running;
	
	uint8_t keyValue_R;
	uint8_t keyValue_B;
	
	uint8_t target_slop;
	bool target_top;
	
	int key_missedPackets;
	int target_missedPackets;
};
#endif // TRACKERS_H_
