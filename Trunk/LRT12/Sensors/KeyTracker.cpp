#include "KeyTracker.h"

#define DEBUG 0

KeyTracker::KeyTracker()
{
	missedPackets = 0;
	running = false;
	disconnect();
	m_task = new Task("KeyTracker::listen", (FUNCPTR)KeyTracker::listenTask);
	m_task->Start((uint32_t) this);
}

KeyTracker::~KeyTracker()
{
	running = false;
	
	m_task->Stop();
	delete m_task;
	
	disconnect();
}

void KeyTracker::listen()
{
	static int counter = 0;
	
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
	
	int lastPacketID = 0;
	
	while(running)
	{
//		if(counter % 10 == 0)
		addr_len = sizeof(client_addr);
		
		int retcode = recvfrom(m_socket, in_buf, sizeof(in_buf), 0, (struct sockaddr *)&client_addr, &addr_len);
		AsyncPrinter::Printf("Yay!\n");
		if (retcode < 0)
		{
			AsyncPrinter::Printf("!!! KeyTracker: Error: recvfrom() failed. %d\n", retcode);
			return;
		}
		
		int pid = ((in_buf[0] << 24) | (in_buf[1] << 16) | (in_buf[2] << 8) | (in_buf[3] << 0));
		keyValue = in_buf[4];

//		if(counter % 10 == 0)
		AsyncPrinter::Printf("Packet ID: %d, value: %d", pid, keyValue);
		
		++counter;
		
		if(pid - 1 != lastPacketID)
		{
			missedPackets = pid - lastPacketID - 1;
		}
		
		lastPacketID = pid;
	}
	AsyncPrinter::Printf("COUNTER : %d\n", counter);
}

uint32_t KeyTracker::getKeyValue()
{
	return keyValue;
}

int KeyTracker::listenTask(uint32_t obj)
{
	KeyTracker *pKeyTracker = (KeyTracker *)obj;
	
	pKeyTracker->running = true;
	pKeyTracker->listen();
	pKeyTracker->running = false;
	
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
	
	AsyncPrinter::Printf("KeyTracker: Socket created.\n");
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8001);
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

void KeyTracker::disconnect()
{
	close(m_socket);
}
