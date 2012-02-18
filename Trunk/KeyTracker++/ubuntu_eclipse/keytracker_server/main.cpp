#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>

using namespace std;

#define writeln(a) cout << a << endl

int main()
{
	int m_socket;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int addr_len;
	unsigned char in_buf[5];
	int retcode;

	int port = 8001;

	writeln("Enter port: ");

	cin >> port;

	writeln("Initializing socket...");

	m_socket = socket(AF_INET, SOCK_DGRAM, 0);

	if(m_socket < 0)
	{
		writeln("Error: socket() failed\n");
		return -1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8001);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	writeln("Binding socket...");

	retcode = bind(m_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

	if(retcode < 0)
	{
		writeln("Error: bind() failed\n");
		return -1;
	}

	writeln("Listening...");

	for(;;)
	{
		addr_len = sizeof(client_addr);
		retcode = recvfrom(m_socket, in_buf, sizeof(in_buf), 0, (struct sockaddr*)&client_addr, (socklen_t*)&addr_len);

		if(retcode < 0)
		{
			writeln("Error: recvfrom() failed\n");
			return -1;
		}

		int pid = ((in_buf[0] << 24) | (in_buf[1] << 16) | (in_buf[2] << 8) | (in_buf[3] << 0));
		int value = in_buf[4];

		writeln("Got pid: " <<  pid << ", value: " << value);
	}
}
