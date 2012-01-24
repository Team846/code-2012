#ifndef LRT_UTIL_H_
#define LRT_UTIL_H_

#include <string>
#include <math.h>
#include <sstream>

/*!
 * @brief Utility class for commonly used mathematical functions
 * @author Robert Ying
 * @author Karthik Viswanathan
 * @author Brian Axelrod
 * @author David Giandomenico
 */

class Util
{
public:
	/*!
	 * @brief Clamps a given value between the min and the max
	 * @param val value to clamp
	 * @param min minimum of that value
	 * @param max maximum of that value
	 * @return clamped value
	 */
	template<typename T> static T Clamp(T val, T min, T max)
	{
		if (val > max)
			return max;
		else if (val < min)
			return min;
		else
			return val;
	}

	/*!
	 * @brief Rescales a value from [min0, max0] to [min1, max1]
	 * @param val value to rescale
	 * @param min0 initial minimum value
	 * @param max0 initial maximum value
	 * @param min1 final minimum value
	 * @param max1 final maximum value
	 * @return the rescaled value
	 */
	template<typename T> static T Rescale(T val, T min0, T max0, T min1, T max1)
	{
		if (max0 == min0)
			return min1;
		val = Clamp<T> (val, min0, max0);
		return (val - min0) * (max1 - min1) / (max0 - min0) + min1;
	}

	/*!
	 * @brief Converts a value to a string
	 * @param val the value to convert
	 * @return the stringified value
	 */
	template<typename T> static std::string ToString(T val)
	{
		std::stringstream ss;
		ss << val;
		return ss.str();
	}

	/*!
	 * @brief Returns the sign of a value
	 * @param val the value to compare
	 * @return the sign of the value
	 */
	template<typename T> static int Sign(T val)
	{
		if (val > 0)
			return 1;
		else if (val < 0)
			return -1;
		return 0;
	}

	/*!
	 * @brief takes the absolute value
	 * @param val the value to absolute
	 * @return the absolute value
	 */
	template<typename T> static T Abs(T val)
	{
		if (val < 0)
			return -val;
		return val;
	}

	/*!
	 * @brief calculates the min, max and mean of an array
	 * @param val the array of values
	 * @param n the number of values in the array
	 * @param minOut a pointer to the minimum
	 * @param maxOut a pointer to the maximum
	 * @param meanOut a pointer to the mean
	 */
	template<typename T> static void MinMaxMean(T val[], int n, T* minOut,
			T* maxOut, T* meanOut)
	{
		if (n == 0)
		{
			*minOut = 0;
			*maxOut = 0;
			*meanOut = 0;
			return;
		}

		T min = val[0], max = val[0];
		T mean = 0;
		for (int i = 0; i < n; ++i)
		{
			mean += val[i];
			if (val[i] < min)
				min = val[i];
			if (val[i] > max)
				max = val[i];
		}
		mean /= n;

		*minOut = min;
		*maxOut = max;
		*meanOut = mean;
	}

	/*!
	 * @brief Adds a deadband into the center of a range
	 * @param raw the raw value from -1 to 1
	 * @param deadbandSize the size of the deadband
	 * @return the value with a deadband
	 */
	template<typename T> static T AddDeadband(T raw, T deadbandSize)
	{
		if (Util::Abs<T>(raw) < deadbandSize)
			return 0;
		return Util::Sign<T>(raw) * Util::Rescale<T>(Util::Abs<T>(raw),
				deadbandSize, 1, 0, 1);
	}

	/*!
	 * @brief Returns value which has a higher absolute
	 * @param val1 first value to compare
	 * @param val2 second value to compare
	 * @return value which is farther from 0
	 */
	template<typename T>
	static T ValWithAbsMax(T val1, T val2)
	{
		if (Util::Abs<T>(val1) > Util::Abs<T>(val2))
			return val1;
		return val2;
	}

	/*!
	 * @brief Returns the higher value
	 * @param val1 first value to compare
	 * @param val2 second value to compare
	 * @return the higher value
	 */
	template<typename T>
	static T Max(T val1, T val2)
	{
		if (val1 > val2)
			return val1;
		return val2;
	}

	/*!
	 * @brief Returns the lower value
	 * @param val1 first value to compare
	 * @param val2 second value to compare
	 * @return the lower value
	 */
	template<typename T>
	static T Min(T val1, T val2)
	{
		if (val1 < val2)
			return val1;
		return val2;
	}

	/*!
	 * @brief Takes the exponent without changing the sign
	 * @param val the base of the exponent
	 * @param power the power of the exponent
	 * @return the exponent
	 */
	template<typename T>
	static T PowPreseveSign(T val, int power)
	{
		T result = pow(val, power);
		if (Sign<T> (val) == Sign<T> (result))
			return result;
		else
			return -result;
	}

	/*!
	 * @brief Kills the current process
	 */
	static void Die();
	
	/*!
	 * @brief Kills the current process with an exit message
	 * @param message
	 */
	static void Die(const char* message);
	
	/*!
	 * @brief Prints the message if the test fails
	 * @param test the test
	 * @param message the message to be printed
	 * @return whether or not the test was passed
	 */
	static bool Assert(bool test, const char* message);
};

#endif
