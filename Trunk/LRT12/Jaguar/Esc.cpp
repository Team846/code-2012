#define MIN(a,b) ((a)>(b)?(b):(a))
#define MAX(a,b) ((a)<(b)?(b):(a))

#include "Esc.h"

// CurrentLimiter class
Esc::CurrentLimiter::CurrentLimiter() :
	timeExtended(0), timeBurst(0), coolExtended(0), coolBurst(0)
{
}

float Esc::CurrentLimiter::Limit(float targetSpeed, float robotSpeed)
{
	//    if(Util::Abs<float>(targetSpeed) < .001)
	//        return 0.0; //dont defeat the dynamic braking
	//
	//    const static float kMaxConst = .55;
	//    if(targetSpeed < 0)
	//        return -Limit(-targetSpeed, - robotSpeed);
	//
	//    float voltage_normalized = DriverStation::GetInstance()->GetBatteryVoltage() / 12;
	//    float voltageLim = kMaxConst / voltage_normalized;
	return targetSpeed;
}

// Esc Class
Esc::Esc(int channel, LRTEncoder& encoder, string name) :
	AsyncCANJaguar(channel, "ESC?"),
			CANJaguarBrake((*(AsyncCANJaguar*) this)),
			m_hasPartner(false), m_partner(NULL), m_encoder(encoder),
			m_name(name), m_index(0), m_stopping_integrator(0),
			m_error_running(0)
{
}

Esc::Esc(int channelA, int channelB, LRTEncoder& encoder, string name) :
	AsyncCANJaguar(channelA, "ESC A?"),
			CANJaguarBrake((*(AsyncCANJaguar*) this)),
			m_hasPartner(true),
			m_partner(new Esc(channelB, encoder, name + "B")),
			m_encoder(encoder), m_name(name + "A"), m_index(0),
			m_stopping_integrator(0), m_error_running(0)
{
	printf("Constructed ESC: %s\n", name.c_str());
}

Esc::~Esc()
{
	if (m_partner)
	{
		delete m_partner;
		m_partner = NULL;
		m_hasPartner = false;
	}
}

void Esc::Configure()
{
	if (m_hasPartner)
		m_partner->Configure();

	string configSection("Esc");
	m_p_gain = Config::GetInstance().Get<float> (configSection, "pGain", 4.0);
}

void Esc::Stop()
{
	if (m_hasPartner)
		m_partner->Stop();

	float RobotSpeed = DriveEncoders::GetInstance().GetNormalizedMotorSpeed(
			m_encoder);
	if (Util::Abs<double>(RobotSpeed) > 0.3)
	{
		SetBrake(8);
		SetDutyCycle(0.0);
		return;
	}
	double error = 0.0 - RobotSpeed;
	//    static float k = 1. / 2;
	//    errorRunning *= k;
	//    errorRunning += error;

	double correction = m_p_gain * m_stopping_integrator.UpdateSum(error);

	//    if(error < 0.01)
	//        Set(0.0);

	//    Set(errorRunning * pGain * (1 - k));
	SetDutyCycle(correction);
	SetBrake(8);
}

void Esc::SetDutyCycle(float speed)

{
	if (m_hasPartner)
		m_partner->SetDutyCycle(speed);

#ifdef USE_DASHBOARD
	//    SmartDashboard::Log(speed, name.c_str());
#endif

	// no current limiting
	AsyncCANJaguar::SetDutyCycle(Util::Clamp<float>(speed, -1.0, 1.0));
	//    controller.Set(channel, Util::Clamp<float>(speed, -1.0, 1.0));
}

void Esc::ResetCache()
{
	AsyncCANJaguar::ResetCache();
	if (m_hasPartner)
		m_partner->ResetCache();
}

void Esc::ApplyBrake()
{
	if (m_hasPartner)
		m_partner->ApplyBrake();

	CANJaguarBrake::ApplyBrakes();
}

void Esc::SetBrake(int brakeAmount)
{
	if (m_hasPartner)
		m_partner->SetBrake(brakeAmount);

	CANJaguarBrake::SetBrake(brakeAmount);
}
