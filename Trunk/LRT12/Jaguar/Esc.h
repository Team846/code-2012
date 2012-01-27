#ifndef ESC_H_
#define ESC_H_

#include "AsyncCANJaguar.h"
#include "CANJaguarBrake.h"
#include "WPILib.h"
#include "..\Config\Config.h"
#include "..\Config\Configurable.h"
#include "..\Util\AsyncPrinter.h"
#include "..\Sensors\DriveEncoders.h"
#include "..\Util\RunningSum.h"
#include <string>
using namespace std;

class Esc: public AsyncCANJaguar, public CANJaguarBrake, public Configurable
{
public:
	Esc(int channel, LRTEncoder& encoder, string name);
	Esc(int channelA, int channelB, LRTEncoder& encoder, string name);
	~Esc();
	virtual void Configure();
	void Stop();
	void SetDutyCycle(float dutycycle);

	void ApplyBrake();
	void SetBrake(int brakeAmount);

	void ResetCache();

private:
	bool m_hasPartner;
	Esc* m_partner;

	class CurrentLimiter
	{
	public:
		CurrentLimiter();
		float Limit(float speed, float robotSpeed);

	private:
		UINT32 timeExtended, timeBurst;
		UINT32 coolExtended, coolBurst;

		const static float kmaxContinous = 40.0 / 133;
		const static float kmaxExtended = 60.0 / 133;
		const static float kmaxBurst = 100.0 / 133;

		const static float kmaxExtendedPeriodSeconds = 2.0;
		const static float kmaxBurstPeriodSeconds = 0.2;

		const static float kminExtendedCooldown = 1.0;
		const static float kminBurstCooldown = 0.5;
	};

	CurrentLimiter m_currentLimiter;
	LRTEncoder& m_encoder;

	string m_name;

	float m_p_gain;
	int m_index;

	const static int kArraySize = 4;
	RunningSum m_stopping_integrator;
	float m_error_running;
	float m_errors[kArraySize];
};

#endif /* ESC_H_ */
