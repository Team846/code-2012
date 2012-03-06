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

/*!
 * @brief Sends data to the cRIO via BSD sockets.
 *
 * @author Tony Peng
 */

class Messenger
{
public:
	/*!
	 * @brief Creates a new instance of the Messenger class. 
	 */
	Messenger();
	/*!
	 * @brief Destructor
	 */
	~Messenger();

	/*!
	 * @brief Sends the data to the cRIO for processing.
	 *
	 * @param pid The packet ID to send.
	 * @param value_r The amount of red pixels detected.
	 * @param value_b The amount of blue pixels detected.
	 */
	int sendData(int pid, int value_r, int value_b);

private:
	/*!
	 * @brief Sets up socket communications.
	 */
	void setup();
	/*!
	 * @brief Disconnects/closes the socket.
	 */
	void disconnect();

	int m_socket;
	struct sockaddr_in m_remote;
};

#endif // MESSENGER_H_
