/*
 * {===================================================================}
 * {| 3.18.2012													      |}
 * {|															      |}
 * {| In memory of the failed communications and the wasted regional. |}
 * {===================================================================}
 */

#ifndef NETWORK_HELPER_H_
#define NETWORK_HELPER_H_

/*!
 * @brief Requests more bandwidth when load balancing is employed.
 * 
 * This class serves no particular purpose but to fix the retarded "aggressive load balancing technology" employed by some routers.
 * How do we do it?
 * We send packets.
 * A lot of packets.
 * A LOT of packets.
 * (And in the process, enhance our score)
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <WPILib.h>

#include "AsyncPrinter.h"

#define min(a, b) ((a < b) ? a : b);

class NetworkHelper
{
public:
	static NetworkHelper* Instance();
	static void deleteInstance();
	
	NetworkHelper();
	~NetworkHelper();
	
	void startTask();
	
	void setPacketsToSend(int p);
	void setBytesPerPacket(int b);
	void setTimeBetweenPacket(double t);
	
	int getPacketsToSend();
	int getBytesPerPacket();
	double getTimeBetweenPacket();
	
	bool sendRequest;
	
private:
	int setup();
	void disconnect();
	
	static void requestTask(uint32_t obj);
	void requestBandwidth();
	
	bool m_is_running;
	
	int m_socket;
	struct sockaddr_in m_remote;
	
	int m_packetsToSend;
	int m_bytesPerPacket;
	
	double m_timeBetweenPackets;
	
	Task *m_task;
	
	static NetworkHelper *instance;
};

#endif // NETWORK_HELPER_H_
