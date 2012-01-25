#ifndef LRT_ENCODER_H_
#define LRT_ENCODER_H_

#include "WPILib.h"
/*!
 * @brief LRTEncoder patches bugs in WPILib's Encoder class.
 */
class LRTEncoder: public Encoder
{
private:
	//  static int count = 0;
	//    Encoder useless;
	float trim;

public:
	/*!
	 * @brief Constructs an LRT Encoder
	 * @param sourceA channel A
	 * @param sourceB channel B
	 * @param trim scaling factor for the encoder
	 */
	LRTEncoder(UINT32 sourceA, UINT32 sourceB, float trim = 1.0);

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

};

#endif
