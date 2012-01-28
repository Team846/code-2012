#ifndef LRT_ENCODER_H_
#define LRT_ENCODER_H_

#include "WPILib.h"
#include "../Log/Loggable.h"

/*!
 * @brief LRTEncoder patches bugs in WPILib's Encoder class.
 */
class LRTEncoder: public Encoder, public Loggable
{
private:
	//  static int count = 0;
	//    Encoder useless;
	float trim;
	std::string m_name;

public:
	/*!
	 * @brief Constructs an LRT Encoder
	 * @param name name of encoder
	 * @param sourceA channel A
	 * @param sourceB channel B
	 * @param trim scaling factor for the encoder
	 */
	LRTEncoder(const char * name, UINT32 sourceA, UINT32 sourceB,
			float trim = 1.0);

	/*!
	 * @brief destroys this LRT Encoder
	 */
	~LRTEncoder();

	/*!
	 * @brief Gets the current count of the encoder
	 * @return encoder tick count
	 */
	INT32 Get();

	/*!
	 * @brief Gets the current rate of the encoder
	 * @return encoder tick rate
	 */
	double GetRate();

	virtual void log();
};

#endif