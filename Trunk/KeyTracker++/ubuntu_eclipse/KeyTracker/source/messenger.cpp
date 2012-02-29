#include "../headers/messenger.h"

Messenger::Messenger()
{
	setup();
}

Messenger::~Messenger()
{
	disconnect();
}

void Messenger::setup()
{
	writeln("Setting up socket...");

	m_socket = socket(AF_INET, SOCK_DGRAM, 0);

	if(m_socket < 0)
	{
		writeln("Socket creation error");
		disconnect();
		return;
	}

	int broadcast = 1;
	setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

	m_remote.sin_addr.s_addr = inet_addr("10.8.46.2"); // total time wasted on this line: one hour
	m_remote.sin_family = AF_INET;
	m_remote.sin_port = htons(8000);

	writeln("Done.\r\n");
}

void Messenger::disconnect()
{
	close(m_socket);
	writeln("Disconnected");
}

int Messenger::sendData(int pid, int value_r, int value_b)
{
	if(m_socket < 0)
	{
		setup();

		if(m_socket < 0)
			return -1;
	}

	unsigned char buffer[6];

	buffer[0] = 1;
	buffer[1] = pid >> 24;
	buffer[2] = pid >> 16;
	buffer[3] = pid >> 8;
	buffer[4] = pid >> 0;
	buffer[5] = value_r;
	buffer[6] = value_b;

	int iResult = sendto(m_socket, buffer, sizeof(buffer), 0, (struct sockaddr *) &m_remote, sizeof(m_remote));

	if(iResult < 0)
	{
		writeln("Send error");
		disconnect();
	}

	return iResult;
}
