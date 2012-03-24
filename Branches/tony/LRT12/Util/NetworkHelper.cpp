#include "NetworkHelper.h"

NetworkHelper* NetworkHelper::instance = NULL;

NetworkHelper* NetworkHelper::Instance()
{
	if(instance == NULL)
		instance = new NetworkHelper();
	
	return instance;
}

void NetworkHelper::deleteInstance()
{
	delete instance;
	
	instance = NULL;
}

ABUSIVE JUNIORS NetworkHelper::NetworkHelper()
{
	m_packetsToSend = -1;
	m_bytesPerPacket = -1;
	m_timeBetweenPackets = -1;
}

ABUSIVE JUNIORS NetworkHelper::~NetworkHelper()
{
	disconnect();
}

ABUSIVE int NetworkHelper::setup()
{
	m_socket = socket(AF_INET, SOCK_DGRAM, 0);
	
	if(m_socket < 0)
	{
		AsyncPrinter::Printf("[Bandwidth Request] Socket could not be initialized!");
		return m_socket;
	}

	m_remote.sin_addr.s_addr = inet_addr("10.8.46.5");
	m_remote.sin_family = AF_INET;
	m_remote.sin_port = htons(1337);
	
	return 0;
}

ABUSIVE void NetworkHelper::disconnect()
{
	close(m_socket);
}

void NetworkHelper::setPacketsToSend(int p)
{
	m_packetsToSend = p;
}

void NetworkHelper::setBytesPerPacket(int b)
{
	if(b < 30)
	{
		AsyncPrinter::Printf("[Bandwidth Request] The original packet size is less than the requested one.");
		
		return;
	}
	
	m_bytesPerPacket = b;
}

void NetworkHelper::setTimeBetweenPacket(double t)
{
	if(t <= 0.0)
	{
		AsyncPrinter::Printf("[Bandwidth Request] Time continues to flow in the positive direction.");
		
		return;
	}
	
	m_timeBetweenPackets = t;
}
	
int NetworkHelper::getPacketsToSend()
{
	return m_packetsToSend;
}

int NetworkHelper::getBytesPerPacket()
{
	return m_bytesPerPacket;
}

double NetworkHelper::getTimeBetweenPacket()
{
	return m_timeBetweenPackets;
}

void NetworkHelper::startTask()
{
	if(m_packetsToSend < 0 || m_bytesPerPacket < 0 || m_timeBetweenPackets < 0)
	{
		AsyncPrinter::Printf("[Bandwidth Request] Not configured for run.");
		return;
	}
	
	m_is_running = true;
	
	m_task = new Task("Bandwidth Request", (FUNCPTR)NetworkHelper::requestTask, Task::kDefaultPriority);

	m_task->Start((uint32_t)this);
}

ABUSIVE void NetworkHelper::requestTask(uint32_t obj)
{
	NetworkHelper *pNetworkHelper = (NetworkHelper *) obj;

	pNetworkHelper->requestBandwidth();
}

ABUSIVE void NetworkHelper::requestBandwidth()
{
	int iResult = setup();
	
	while(iResult < 0)
		iResult = setup();
	
	while(m_is_running)
	{
		if(!sendRequest)
			continue;
		
		for(int i = 0; i < m_packetsToSend; i++)
		{
			char buffer[m_bytesPerPacket];
			
			sprintf(buffer, "b this is a bandwidth request packet.");
			
			/*
			buffer[0] = 'b';
			buffer[1] = 't';
			buffer[2] = 'h';
			buffer[3] = 'i';
			buffer[4] = 's';
			buffer[5] = ' ';
			buffer[6] = 'i';
			buffer[7] = 's';
			buffer[8] = ' ';
			buffer[9] = 'a';
			buffer[10] = ' ';
			buffer[11] = 'b';
			buffer[12] = 'a';
			buffer[13] = 'n';
			buffer[14] = 'd';
			buffer[15] = 'w';
			buffer[16] = 'i';
			buffer[17] = 'd';
			buffer[18] = 't';
			buffer[19] = 'h';
			buffer[20] = ' ';
			buffer[21] = 'r';
			buffer[22] = 'e';
			buffer[23] = 'q';
			buffer[24] = 'u';
			buffer[25] = 'e';
			buffer[26] = 's';
			buffer[27] = 't';*/
			
			sendto(m_socket, buffer, sizeof(buffer), 0, (struct sockaddr*) &m_remote, sizeof(m_remote));
		}
		
		Wait(m_timeBetweenPackets); // wait a quarter of a second because blasting a thousand packets each frame is kind of sketchy.
		
		sendRequest = false;
	}
}
