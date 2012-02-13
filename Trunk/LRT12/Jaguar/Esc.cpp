#define MIN(a,b) ((a)>(b)?(b):(a))
#define MAX(a,b) ((a)<(b)?(b):(a))

#include "Esc.h"

/************************** Esc Class ********************/
ESC::ESC(int channel, LRTEncoder *encoder, string name) :
	m_name(name)
{
	m_encoder = encoder;
	m_jag1 = new AsyncCANJaguar(channel, name.c_str());
	m_jag2 = NULL;
	m_cycle_count = 0;
}

ESC::ESC(int channelA, int channelB, LRTEncoder* encoder, string name) :
	m_name(name)
{
	m_encoder = encoder;
	std::string tempa, tempb;
	tempa = name + "A";
	tempb = name + "B";
	m_jag1 = new AsyncCANJaguar(channelA, tempa.c_str());
	m_jag2 = new AsyncCANJaguar(channelB, tempb.c_str());
	
	m_jag1->ConfigNeutralMode(AsyncCANJaguar::kNeutralMode_Coast);
	m_jag2->ConfigNeutralMode(AsyncCANJaguar::kNeutralMode_Coast);
	m_cycle_count = 0;
	printf("Constructed ESC: %s\n", name.c_str());
}

ESC::~ESC()
{
	delete m_jag1;
	delete m_jag2;
}

void ESC::Configure()
{
	string configSection("Esc");
}

//first is dutycycle, second is braking
std::pair<float, float> ESC::CalculateBrakeAndDutyCycle(float desired_speed,
		float current_speed)
{
	std::pair<float, float> command;

	command.first = 0.0;
	command.second = 0.0;

	if (current_speed < 0)
	{
		command = CalculateBrakeAndDutyCycle(-desired_speed, -current_speed);
		command.first = -command.first;
		return command;
	}

	// speed >= 0 at this point
	if (desired_speed >= current_speed) // trying to go faster
	{
		command.first = desired_speed;
		command.second = 0.0;
	}
	// trying to slow down
	else
	{
		float error = desired_speed - current_speed; // error always <= 0

		if (desired_speed >= 0) // braking is based on speed alone; reverse power unnecessary
		{
			command.first = 0.0; // must set 0 to brake

			if (current_speed > -error)
				command.second = -error / current_speed; // speed always > 0
			else
				command.second = 1.0;
		}
		else // input < 0; braking with reverse power
		{
			command.second = 0.0; // not braking
			command.first = error / (1.0 + current_speed); // dutyCycle <= 0 because error <= 0
		}
	}

	return command;
}

void ESC::SetDutyCycle(float dutyCycle)
{
#ifdef USE_DASHBOARD
	//    SmartDashboard::Log(speed, name.c_str());
#endif
	double speed = m_encoder->GetRate() / DriveEncoders::GetInstance().getMaxEncoderRate();
	std::pair<float, float> command = CalculateBrakeAndDutyCycle(dutyCycle,
			speed);

	if (fabs(command.first) < 1E-4) //brake only when duty cycle = 0
	{
		dutyCycle = 0.0;

		// cycleCount ranges from 0 to 8
		if (++m_cycle_count >= 8)
			m_cycle_count = 0;

		/*
		 * Each integer, corresponding to value, is a bitfield of 8 cycles
		 * Pattern N has N bits out of 8 set to true.
		 * 0: 0000 0000 = 0x00
		 * 1: 0000 0001 = 0x01
		 * 2: 0001 0001 = 0x11
		 * 3: 0010 0101 = 0x25
		 * 4: 0101 0101 = 0x55
		 * 5: 1101 0101 = 0xD5
		 * 6: 1110 1110 = 0xEE
		 * 7: 1111 1110 = 0xFE
		 * 8: 1111 1111 = 0xFF
		 */
		static const UINT8 ditherPattern[] =
		{ 0x00, 0x01, 0x11, 0x25, 0x55, 0xD5, 0xEE, 0xFE, 0xFF };

		int brake_level = (int) (fabs(command.second) * 8);
		bool shouldBrakeThisCycle = ditherPattern[brake_level] & (1
				<< m_cycle_count);

		if (shouldBrakeThisCycle)
		{
			m_jag1->ConfigNeutralMode(AsyncCANJaguar::kNeutralMode_Brake);
			m_jag2->ConfigNeutralMode(AsyncCANJaguar::kNeutralMode_Brake);
		}
		else
		{
			m_jag1->ConfigNeutralMode(AsyncCANJaguar::kNeutralMode_Coast);
			m_jag2->ConfigNeutralMode(AsyncCANJaguar::kNeutralMode_Coast);
		}
	}

	dutyCycle = Util::Clamp<float>(dutyCycle, -1.0, 1.0);

	static int e = 0;
	if ((e++)%21 == 0)
		AsyncPrinter::Printf("In: %.4f out %.4f speed %.4f\n", dutyCycle, command.first, speed);
	m_jag1->SetDutyCycle(command.first);
	m_jag2->SetDutyCycle(command.first);
//	m_jag1->Set(command.first);
//	m_jag2->Set(command.first);
}

void ESC::ResetCache()
{
	m_jag1->ResetCache();
	if (m_jag2)
		m_jag2->ResetCache();
}

