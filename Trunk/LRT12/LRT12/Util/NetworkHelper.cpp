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

NetworkHelper::NetworkHelper()
{
	m_packetsToSend = -1;
	m_bytesPerPacket = -1;
	m_timeBetweenPackets = -1;
}

NetworkHelper::~NetworkHelper()
{
	disconnect();
}

int NetworkHelper::setup()
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

void NetworkHelper::disconnect()
{
	close(m_socket);
}

void NetworkHelper::setPacketsToSend(int p)
{
	m_packetsToSend = p;
}

void NetworkHelper::setBytesPerPacket(int b)
{
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

void NetworkHelper::requestTask(uint32_t obj)
{
	NetworkHelper *pNetworkHelper = (NetworkHelper *) obj;

	pNetworkHelper->requestBandwidth();
}

void NetworkHelper::requestBandwidth()
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
			
			int upperBound = ((i > 255) ? 255 : i);
			
			for(int j = 0; j < upperBound; j++)
			{
				buffer[j] = i;
			}
			
			sendto(m_socket, buffer, sizeof(buffer), 0, (struct sockaddr*) &m_remote, sizeof(m_remote));
		}
		
		Wait(m_timeBetweenPackets); // wait some time because blasting a thousand packets each frame is...suspicious.
		
		sendRequest = false;
	}
}
