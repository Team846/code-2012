#ifndef MESSENGER_H_
#define MESSENGER_H_

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

using namespace std;

#define writeln(a) cout << a << endl

class Messenger
{
public:
	Messenger();
	~Messenger();

	int sendData(int pid, int value_r, int value_b);

private:
	void setup();
	void disconnect();

	int m_socket;
	struct sockaddr_in m_remote;
};

#endif // MESSENGER_H_
