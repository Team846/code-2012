#ifndef RUNNING_SUM_H_
#define RUNNING_SUM_H_

/*!
 * @brief Maintains a running sum
 */

class RunningSum
{
public:
	/*!
	 * @brief Constructs a running sum
	 * @param decayConstant rate of decay
	 */
	RunningSum(double decayConstant) :
		decayConstant(decayConstant), runningSum(0)
	{

	}

	/*!
	 * @brief Adds a value into the sum
	 * @param x
	 * @return new value
	 */
	double UpdateSum(double x)
	{
		runningSum *= decayConstant;
		runningSum += x;
		return runningSum * (1 - decayConstant);
	}

	/*!
	 * @brief clears the running sum
	 */
	void Clear()
	{
		runningSum = 0;
	}

private:
	double decayConstant;
	double runningSum;
};

#endif
