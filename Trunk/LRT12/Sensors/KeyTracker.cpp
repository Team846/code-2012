#include "KeyTracker.h"

KeyTracker& KeyTracker::instance()
{
	static KeyTracker keyTracker;
	
	return keyTracker;
}

KeyTracker::KeyTracker()
{
	missedPackets = 0;
	running = false;
	
	int iResult = setup();

	if(iResult != 0)
	{
		AsyncPrinter::Printf("!!! KeyTracker: Some errors occurred during initialization.");
	}
	
	m_task = new Task("KeyTracker::listen", (FUNCPTR)KeyTracker::listenTask());
	m_task->Start();
}

KeyTracker::~KeyTracker()
{
	running = false;
	disconnect();
}

void KeyTracker::listen()
{
	int lastPacketID = 0;
	
	while(running)
	{
		addr_len = sizeof(client_addr);
		int retcode = recvfrom(m_socket, in_buf, sizeof(in_buf), 0, (struct sockaddr*)&client_addr, &addr_len);
		
		if (retcode < 0)
		{
			AsyncPrinter::Printf("!!! KeyTracker: Error: recvfrom() failed. %d\n", retcode);
			return;
		}
		
		int pid = ((in_buf[0] << 24) | (in_buf[1] << 16) | (in_buf[2] << 8) | (in_buf[3] << 0));
		keyValue = in_buf[4];
		
		if(pid - 1 != lastPacketID)
		{
			missedPackets = pid - lastPacketID - 1;
		}
		
		lastPacketID = pid;
	}
}

uint32_t KeyTracker::getKeyValue()
{
	return keyValue;
}

int KeyTracker::listenTask()
{
	instance().running = true;
	instance().listen();
	instance().running = false;
	
	return 0;
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
