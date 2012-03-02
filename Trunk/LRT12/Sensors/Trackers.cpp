#include "Trackers.h"

#define DEBUG 0

Trackers::Trackers(int task_priority)
{
	m_key_missed_packets = 0;
	m_target_missed_packets = 0;

	m_is_running = false;
	disconnect();
	m_task = new Task("KeyTracker::listen", (FUNCPTR) Trackers::listenTask,
			task_priority);
	m_task->Start((uint32_t) this);
}

Trackers::~Trackers()
{
	m_is_running = false;

	m_task->Stop();
	delete m_task;

	disconnect();
}

void Trackers::listen()
{
	m_is_running = true;
	m_task_is_done = false;

	AsyncPrinter::Printf("Starting listen task...\n");

	int iResult = -1;

	/* TODO: Change loop to only bind, not setup. */
	while (iResult != 0)
	{
		iResult = setup();

		if (iResult != 0)
		{
			AsyncPrinter::Printf("!!! KeyTracker: Retrying bind...\n");
			Wait(0.5);
		}
	}

	AsyncPrinter::Printf("Socket listening...\n");

	int key_lastPacketID = 0;
	int target_lastPacketID = 0;

	while (m_is_running)
	{
		//		if(counter % 10 == 0)
		m_address_length = sizeof(m_client_address);

		int retcode = recvfrom(m_socket, m_input_buffer,
				sizeof(m_input_buffer), 0,
				(struct sockaddr *) &m_client_address, &m_address_length);

		if (retcode < 0)
		{
			AsyncPrinter::Printf(
					"!!! KeyTracker: Error: recvfrom() failed. %d\n", retcode);
			return;
		}

		uint8_t header = m_input_buffer[0];

		int pid = -1;

		switch (header)
		{
		case 0:
			pid = ((m_input_buffer[1] << 24) | (m_input_buffer[2] << 16)
					| (m_input_buffer[3] << 8) | (m_input_buffer[4] << 0));

			if (pid - 1 != target_lastPacketID)
			{
				m_target_missed_packets = pid - target_lastPacketID - 1;
			}

			m_target_slop = m_input_buffer[5];
			m_target_top = m_input_buffer[6];

			target_lastPacketID = pid;

			break;
		case 1:
			pid = ((m_input_buffer[1] << 24) | (m_input_buffer[2] << 16)
					| (m_input_buffer[3] << 8) | (m_input_buffer[4] << 0));

			if (pid - 1 != key_lastPacketID)
			{
				m_key_missed_packets = pid - key_lastPacketID - 1;
			}

			m_key_value_r = m_input_buffer[5];
			m_key_value_b = m_input_buffer[6];

			key_lastPacketID = pid;
			
			AsyncPrinter::Printf("%d: r: %d, b: %d\n", pid, m_key_value_r, m_key_value_b);
			break;
		}

		//		if(counter % 10 == 0)

		//		++counter;
		update();
	}

	m_task_is_done = true;
}

void Trackers::update()
{
	camera * c = ActionData::GetInstance()->cam;
	c->key.blue = getKeyValue(BLUE);
	c->key.red = getKeyValue(RED);
	c->key.higher = getKeyValue(HIGHER);
	c->align.arbitraryOffsetFromUDP = getTargetSlop();
	c->align.isUpperTarget = getTargetTop();
}

void Trackers::stop(bool force)
{
	if (m_is_running)
	{
		m_is_running = false;

		if (force)
		{
			m_task->Stop();
			m_task_is_done = true;

			return;
		}

		while (!m_task_is_done)
			Wait(0.01);

		m_task->Stop();
	}

	return;
}

uint8_t Trackers::getKeyValue(KeyValue v)
{
	switch (v)
	{
	case HIGHER:
		return (m_key_value_r > m_key_value_b) ? m_key_value_r : m_key_value_b;
		break;
	case LOWER:
		return (m_key_value_r < m_key_value_b) ? m_key_value_r : m_key_value_b;
		break;
	case RED:
		return m_key_value_r;
		break;
	case BLUE:
		return m_key_value_b;
		break;
	}
	return m_key_value_r;
}

uint8_t Trackers::getTargetSlop()
{
	return m_target_slop;
}

bool Trackers::getTargetTop()
{
	return m_target_top;
}

int Trackers::listenTask(uint32_t obj)
{
	Trackers *pKeyTracker = (Trackers *) obj;

	pKeyTracker->listen();

	return 0;
}

int Trackers::setup()
{
	int retcode;

	m_socket = socket(AF_INET, SOCK_DGRAM, 0);

	if (m_socket < 0)
	{
		AsyncPrinter::Printf("!!! KeyTracker: Error: socket() failed. %d\n",
				m_socket);
		return m_socket;
	}

	AsyncPrinter::Printf("KeyTracker: Socket created.\n");

	m_server_address.sin_family = AF_INET;
	m_server_address.sin_port = htons(8000);
	m_server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	char ra = '1';

	setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &ra, sizeof(ra));

	retcode = bind(m_socket, (struct sockaddr *) &m_server_address,
			sizeof(m_server_address));

	if (retcode < 0)
	{
		AsyncPrinter::Printf("!!! KeyTracker: Error: bind() failed. %d\n",
				retcode);
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
