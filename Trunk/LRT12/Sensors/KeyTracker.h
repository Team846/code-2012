#ifndef KEY_TRACKER_H_
#define KEY_TRACKER_H_

#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>

using namespace std;

class KeyTracker : public Configurable, public Loggable
{
public:
	KeyTracker();
	~KeyTracker();
	
	void listen();
	
private:
	void setup();
	void disconnect();
	
	int m_socket;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int addr_len;
	unsigned char in_buf[5];
};

#endif // KEY_TRACKER_H_
