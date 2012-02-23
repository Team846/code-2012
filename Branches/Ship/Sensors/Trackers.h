#ifndef KEY_TRACKER_H_
#define KEY_TRACKER_H_
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

class Trackers
{
public:
	Trackers();
	~Trackers();

	static int listenTask(uint32_t obj);
	
	void listen();
	
	uint8_t getKeyValue();
	uint8_t getTargetSlop();
	bool getTargetTop();
	
private:
	
	int setup();
	void disconnect();
	
	Task *m_task;
	
	int m_socket;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int addr_len;
	char in_buf[7];
	bool running;
	
	uint8_t keyValue;
	uint8_t target_slop;
	bool target_top;
	
	int key_missedPackets;
	int target_missedPackets;
};
#endif // KEY_TRACKER_H_
