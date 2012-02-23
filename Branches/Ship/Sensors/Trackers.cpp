#include "Trackers.h"

#define DEBUG 0

Trackers::Trackers()
{
	key_missedPackets = 0;
	target_missedPackets = 0;
	
	running = false;
	disconnect();
	m_task = new Task("KeyTracker::listen", (FUNCPTR)Trackers::listenTask);
	m_task->Start((uint32_t) this);
}

Trackers::~Trackers()
{
	running = false;
	
	m_task->Stop();
	delete m_task;
	disconnect();
}

void Trackers::listen()
{
	AsyncPrinter::Printf("Starting listen task...\n");
	
	int iResult = -1;
	
	/* TODO: Change loop to only bind, not setup. */
	while(iResult != 0)
	{
		iResult = setup();
	
		if(iResult != 0)
		{
			AsyncPrinter::Printf("!!! KeyTracker: Retrying bind...\n");
			Wait(0.5);
		}
	}
	
	AsyncPrinter::Printf("Socket listening...\n");
	
	int key_lastPacketID = 0;
	int target_lastPacketID = 0;
	
	while(running)
	{
//		if(counter % 10 == 0)
		addr_len = sizeof(client_addr);
		
		int retcode = recvfrom(m_socket, in_buf, sizeof(in_buf), 0, (struct sockaddr *)&client_addr, &addr_len);
		
		if (retcode < 0)
		{
			AsyncPrinter::Printf("!!! KeyTracker: Error: recvfrom() failed. %d\n", retcode);
			return;
		}
		
		uint8_t header = in_buf[0];
		
		int pid = -1;
		
		switch(header)
		{
			case 0:
				pid = ((in_buf[1] << 24) | (in_buf[2] << 16) | (in_buf[3] << 8) | (in_buf[4] << 0));
				
				if(pid - 1 != target_lastPacketID)
				{
					target_missedPackets = pid - target_lastPacketID - 1;
				}
				
				target_slop = in_buf[5];
				target_top = in_buf[6];
				
				target_lastPacketID = pid;
				
				break;
			case 1:
				pid = ((in_buf[1] << 24) | (in_buf[2] << 16) | (in_buf[3] << 8) | (in_buf[4] << 0));
				
				if(pid - 1 != key_lastPacketID)
				{
					key_missedPackets = pid - key_lastPacketID - 1;
				}
				
				keyValue = in_buf[5];
				
				key_lastPacketID = pid;
			break;
		}

//		if(counter % 10 == 0)
		
//		++counter;
	}
}

uint8_t Trackers::getKeyValue()
{
	return keyValue;
}

uint8_t Trackers::getTargetSlop()
{
	return target_slop;
}

bool Trackers::getTargetTop()
{
	return target_top;
}

int Trackers::listenTask(uint32_t obj)
{
	Trackers *pKeyTracker = (Trackers *)obj;
	
	pKeyTracker->running = true;
	pKeyTracker->listen();
	pKeyTracker->running = false;
	
	return 0;
}

int Trackers::setup()
{
	int retcode;
	
	m_socket = socket(AF_INET, SOCK_DGRAM, 0);
	
	if(m_socket < 0)
	{
		AsyncPrinter::Printf("!!! KeyTracker: Error: socket() failed. %d\n", m_socket);
		return m_socket;
	}
	
	AsyncPrinter::Printf("KeyTracker: Socket created.\n");
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8000);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	char ra = '1';
	
	setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &ra, sizeof(ra));
	
	retcode = bind(m_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
	
	if(retcode < 0)
	{
		AsyncPrinter::Printf("!!! KeyTracker: Error: bind() failed. %d\n", retcode);
		return retcode;
	}
	
	AsyncPrinter::Printf("KeyTracker: Socket successfully binded.\n");
	
	AsyncPrinter::Printf("Socket successfully initialized.\n");
	
	return 0;
}

void Trackers::disconnect()
{
	close(m_socket);
}
