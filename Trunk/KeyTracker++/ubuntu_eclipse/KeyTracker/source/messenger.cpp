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

	m_remote.sin_addr.s_addr = inet_addr("127.0.0.1");
	m_remote.sin_family = AF_INET;
	m_remote.sin_port = htons(8001);

	writeln("Done.\r\n");
}

void Messenger::disconnect()
{
	close(m_socket);
	writeln("Disconnected");
}

int Messenger::sendData(int pid, int value)
{
	if(m_socket < 0)
	{
		setup();

		if(m_socket < 0)
			return -1;
	}

	unsigned char buffer[5];

	buffer[0] = pid >> 24;
	buffer[1] = pid >> 16;
	buffer[2] = pid >> 8;
	buffer[3] = pid >> 0;
	buffer[4] = value;

	int iResult = sendto(m_socket, buffer, sizeof(buffer), 0, (struct sockaddr *) &m_remote, sizeof(m_remote));

	if(iResult < 0)
	{
		writeln("Send error");
		disconnect();
	}

	return iResult;
}
