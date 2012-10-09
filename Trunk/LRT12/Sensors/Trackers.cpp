#include "Trackers.h"
#include <math.h>

#define DEBUG 0

Trackers::Trackers(int task_priority) :
	Loggable(),
	m_indicator_r(5),
	m_indicator_g(6),
	m_indicator_b(7)
{
	m_key_missed_packets = 0;
	m_target_missed_packets = 0;

	m_key_value_r = 0;
	m_key_value_b = 0;

	m_indicator_r.Set(0);
	m_indicator_g.Set(0);
	m_indicator_b.Set(0);

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
		int lowestYIndex = 0;
		int ms_to_process = 0;
		uint32_t *tmp = 0;
		
		switch (header)
		{
		case 0:
			pid = m_input_buffer[1];

			if (pid - 1 != target_lastPacketID)
			{
				m_target_missed_packets = pid - target_lastPacketID - 1;
			}
			int numTargets = m_input_buffer[2];
			ms_to_process = m_input_buffer[3];
			
			TargetInfo *targets = new TargetInfo[numTargets];
			
			if (numTargets == 0){
				m_indicator_r.Set(0);
				break;
			}
			m_indicator_r.Set(1);
			
			tmp = (uint32_t *) &m_input_buffer[4];
//			AsyncPrinter::Printf("num Targets: %d Time to execute : %d ms\n", numTargets, ms_to_process);
			for (int i = 0; i < numTargets; i++)
			{
				targets[i].x = ntohl(*tmp);
				tmp++;
				targets[i].y = ntohl(*tmp);
				tmp++;

				targets[i].distance = ntohl(*tmp);
				tmp++;
				
//				AsyncPrinter::Printf("%d: x: %d y: %d dist:%d \n", i, targets[i].x, targets[i].y, targets[i].y);
			}
			//take the highest square (lowest y)
			for (int i = 1; i < numTargets; i++)
			{
				if (targets[i].y < targets[lowestYIndex].y)
					lowestYIndex = i;																					
			}
			
//			AsyncPrinter::Printf("target: %d\n", lowestYIndex);
			m_target_x = targets[lowestYIndex].x;
			m_target_dist = targets[lowestYIndex].distance;
			ActionData::GetInstance()->cam->align.hasNewData = true;
			target_lastPacketID = pid;

			break;
		case 1:
			pid = m_input_buffer[1];

			if (pid - 1 != target_lastPacketID)
			{
				m_target_missed_packets = pid - target_lastPacketID - 1;
			}
			int numBalls = m_input_buffer[2];
			int ms_to_process = m_input_buffer[3];
			
			BallInfo *balls = new BallInfo[numBalls];
			
			if (numTargets == 0)
				break;
			
			uint32_t *tmp = (uint32_t *) &m_input_buffer[4];
			for (int i = 0; i < numBalls; i++)
			{
				balls[i].x = ntohl(*tmp);
				tmp++;
				balls[i].y = ntohl(*tmp);
				tmp++;

				balls[i].radius = ntohl(*tmp);
				tmp++;
			}
			
			if(numBalls > 0)
				m_indicator_b.Set(1);
			else
				m_indicator_b.Set(0);
			
			
			//For now just print out the output
//			for (int i = 0; i < numBalls; i++)
//			{
//					AsyncPrinter::Printf("index %d: x: %d y: %d radius:%d \n", i, balls[i].x, balls[i].y, balls[i].radius);
//			}
			target_lastPacketID = pid;
			
			{
				Synchronized s(ActionData::GetInstance()->cam->ballSem);
				ActionData::GetInstance()->cam->numDetectedBalls = numBalls;
				for (int i = 0; i < numBalls; i++)
				{
					ActionData::GetInstance()->cam->balls[i] = balls[i];
//					AsyncPrinter::Printf("index %d: x: %d y: %d radius:%d \n", i, balls[i].x, balls[i].y, balls[i].radius);
//					AsyncPrinter::Printf("index %d: x: %d y: %d radius:%d \n", i, 
//							ActionData::GetInstance()->cam->balls[i].x, 
//							ActionData::GetInstance()->cam->balls[i].y,
//							ActionData::GetInstance()->cam->balls[i].radius);
				}
				ActionData::GetInstance()->cam->hasBeenUpdated = true;;
			}
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
	CameraData * c = ActionData::GetInstance()->cam;
	c->key.blue = getKeyValue(BLUE);
	c->key.red = getKeyValue(RED);
	c->key.higher = getKeyValue(HIGHER);
	
	c->align.arbitraryOffsetFromUDP = getTargetOffset();
	c->align.distance = getTargetDistance();
	if (c->align.arbitraryOffsetFromUDP == ACTION::CAMERA::INVALID_DATA)
	{
		c->align.status = ACTION::CAMERA::NO_TARGET;
	}
	else
	{
		c->align.status = getSelectedTarget() ? ACTION::CAMERA::TOP
				: ACTION::CAMERA::BOTTOM;
	}
	//	c->align.canSeeTarget = c->align.arbitraryOffsetFromUDP != 128;
	//		
	//	c->align.isUpperTarget = getSelectedTarget();
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

uint32_t Trackers::getTargetOffset()
{
	return m_target_x;
}

uint32_t Trackers::getTargetDistance()
{
	return m_target_dist;
}


bool Trackers::getSelectedTarget()
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

void Trackers::log()
{
#if LOGGING_ENABLED
	SmartDashboard *sdb = SmartDashboard::GetInstance();
	CameraData * c = ActionData::GetInstance()->cam;
	sdb->PutInt("Key Blue", c->key.blue);
	sdb->PutInt("Key Red", c->key.red);
	sdb->PutInt("Key Higher", c->key.higher);
	sdb->PutInt("Target Offset", c->align.arbitraryOffsetFromUDP);
	string s;
	switch (c->align.status)
	{
		case ACTION::CAMERA::NO_TARGET:
		s = "None";
		break;
		case ACTION::CAMERA::TOP:
		s = "Top";
		break;
		case ACTION::CAMERA::BOTTOM:
		s = "Bottom";
		break;
	}
	sdb->PutString("Target Selected", s.c_str());
#endif
}
