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

class KeyTracker
{
public:
	KeyTracker();
	~KeyTracker();

	static int listenTask(uint32_t obj);
	
	void listen();
	
	uint32_t getKeyValue();
	
private:
	
	int setup();
	void disconnect();
	
	Task *m_task;
	
	int m_socket;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int addr_len;
	char in_buf[5];
	bool running;
	
	uint32_t keyValue;
	int missedPackets;
};
#endif // KEY_TRACKER_H_
