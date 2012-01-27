#ifndef CACHED_VALUE_H_
#define CACHED_VALUE_H_

/*!
 * @brief Basic class to keep track of cached values
 * @author Robert Ying
 */

class CachedFloat
{
public:
	/*!
	 * @brief Constructs a new cached value object with an initial value
	 * @param initialValue the initial cached value
	 * @param cacheCycles the number of cycles before automatic refresh
	 */
	CachedFloat(float initialValue, int cacheCycles = 25);

	/*!
	 * @brief Constructs a new cached value object
	 */
	CachedFloat();

	/*!
	 * @brief Set a new value to be cached, automatically sets the new value flag
	 * @param newValue the new value to be cached
	 */
	void setValue(float newValue);

	/*!
	 * @brief Gets the most recent cached value and clears the new value flag 
	 * @return the cached value
	 */
	float getValue();

	/*!
	 * @brief Forcibly sets the new value flag.
	 */
	void uncache();

	/*!
	 * @brief Increments the internal cycle counter
	 */
	void incrementCounter();

	/*!
	 * @brief Enables caching, defaults to previous setting of cache cycles
	 * @param cacheCycles number of cycles to cache
	 */
	void enableCaching(int cacheCycles = -1);

	/*!
	 * @brief Disables caching
	 */
	void disableCaching();

	/*!
	 * Flag for new or updated cache value
	 * @return whether or not the cached value has changed
	 */
	bool hasNewValue();

	/*!
	 * Flag for whether or not caching is enabled
	 * @return whether or not caching is enabled
	 */
	bool isCaching();

private:
	volatile float m_value;
	volatile float m_previous_value;
	bool m_has_new_value;
	bool m_is_caching;
	bool m_has_been_set;
	int m_counter;
	int m_cache_cycles;
};

/*!
 * @brief Basic class to keep track of cached values
 * @author Robert Ying
 */

class CachedInt
{
public:
	/*!
	 * @brief Constructs a new cached value object with an initial value
	 * @param initialValue the initial cached value
	 * @param cacheCycles the number of cycles before automatic refresh
	 */
	CachedInt(int initialValue, int cacheCycles = 25);

	/*!
	 * @brief Constructs a new cached value object
	 */
	CachedInt();

	/*!
	 * @brief Set a new value to be cached, automatically sets the new value flag
	 * @param newValue the new value to be cached
	 */
	void setValue(int newValue);

	/*!
	 * @brief Gets the most recent cached value and clears the new value flag 
	 * @return the cached value
	 */
	int getValue();

	/*!
	 * @brief Forcibly sets the new value flag.
	 */
	void uncache();

	/*!
	 * @brief Increments the internal cycle counter
	 */
	void incrementCounter();

	/*!
	 * @brief Enables caching, defaults to previous setting of cache cycles
	 * @param cacheCycles number of cycles to cache
	 */
	void enableCaching(int cacheCycles = -1);

	/*!
	 * @brief Disables caching
	 */
	void disableCaching();

	/*!
	 * Flag for new or updated cache value
	 * @return whether or not the cached value has changed
	 */
	bool hasNewValue();

	/*!
	 * Flag for whether or not caching is enabled
	 * @return whether or not caching is enabled
	 */
	bool isCaching();

private:
	volatile int m_value;
	volatile int m_previous_value;
	bool m_has_new_value;
	bool m_is_caching;
	bool m_has_been_set;
	int m_counter;
	int m_cache_cycles;
};

#endif
