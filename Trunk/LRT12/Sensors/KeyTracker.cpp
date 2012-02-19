#include "KeyTracker.h"

KeyTracker::KeyTracker()
{
	int iResult = setup();

	if(iResult != 0)
	{
		AsyncPrinter::Printf("!!! KeyTracker: Some errors occurred during initialization.");
	}
}

void KeyTracker::listen()
{
	addr_len = sizeof(client_addr);
	int retcode = recvfrom(m_socket, in_buf, sizeof(in_buf), 0, (struct sockaddr*)&client_addr, (socklen_t*)&addr_len);
	
	if (retcode < 0)
	{
		AsyncPrinter::Printf("!!! KeyTracker: Error: recvfrom() failed. %d\n", retcode);
		return;
	}
	
	int pid = ((in_buf[0] << 24) | (in_buf[1] << 16) | (in_buf[2] << 8) | (in_buf[3] << 0));
	int value = in_buf[4];
	
	int speed = value / 255;
}

int KeyTracker::setup()
{
	int retcode;
	
	m_socket = socket(AF_INET, SOCK_DGRAM, 0);
	
	if(m_socket < 0)
	{
		AsyncPrinter::Printf("!!! KeyTracker: Error: socket() failed. %d\n", m_socket);
		return m_socket;
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8001);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	retcode = bind(m_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
	
	if(retcode < 0)
	{
		AsyncPrinter::Printf("!!! KeyTracker: Error: bind() failed. %d\n", retcode);
		return retcode;
	}
	
	return 0;
}

void KeyTracker::disconnect()
{
	close(m_socket);
}
