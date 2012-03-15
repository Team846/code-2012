#ifndef CACHED_VALUE_H_
#define CACHED_VALUE_H_

/*!
 * @brief Basic class to keep track of cached values
 * @author Robert Ying
 */

template<class T>
class CachedValue
{
public:
	/*!
	 * @brief Constructs a new cached value object with an initial value
	 * @param initialValue the initial cached value
	 * @param cacheCycles the number of cycles before automatic refresh
	 */
	CachedValue(T initialValue, int cacheCycles = 12);

	/*!
	 * @brief Constructs a new cached value object
	 */
	CachedValue();

	/*!
	 * @brief Set a new value to be cached, automatically sets the new value flag
	 * @param newValue the new value to be cached
	 */
	void setValue(T newValue);

	/*!
	 * @brief Gets the most recent cached value and clears the new value flag 
	 * @return the cached value
	 */
	T getValue();

	/*!
	 * @brief Gets the most recent cached value
	 * @return
	 */
	T peek();

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
	 * Flag for existance of value
	 * @return whether or not the cached value has set
	 */
	bool hasValue();

	/*!
	 * Flag for whether or not caching is enabled
	 * @return whether or not caching is enabled
	 */
	bool isCaching();

private:
	volatile T m_value;
	volatile T m_previous_value;
	bool m_has_new_value;
	bool m_is_caching;
	bool m_has_been_set;
	int m_counter;
	int m_cache_cycles;
};

template<class T>
CachedValue<T>::CachedValue(T initialValue, int cacheCycles)
{
	m_value = initialValue;
	m_cache_cycles = cacheCycles;
	enableCaching(cacheCycles);
	uncache();
	m_has_been_set = true;
}

template<class T>
CachedValue<T>::CachedValue()
{
	enableCaching(12);
	m_has_new_value = false;
	m_has_been_set = false;
	m_counter = 0;
	m_previous_value = T();
	m_value = T();
}

template<class T>
void CachedValue<T>::setValue(T newValue)
{
	m_has_been_set = true;
	// value is already cached, ignore input
	if (m_previous_value == newValue)
	{
		return;
	}

	// set new item flag
	m_value = newValue;
	uncache();
}

template<class T>
T CachedValue<T>::getValue()
{
	m_has_new_value = false;
	m_previous_value = m_value;
	return m_value;
}

template<class T>
T CachedValue<T>::peek()
{
	return m_value;
}

template<class T>
void CachedValue<T>::uncache()
{
	m_has_new_value = true;
	m_counter = 0;
}

template<class T>
void CachedValue<T>::incrementCounter()
{
	if (!m_is_caching || ++m_counter >= m_cache_cycles)
	{
		uncache();
	}
}

template<class T>
void CachedValue<T>::enableCaching(int cacheCycles)
{
	m_is_caching = true;

	if (cacheCycles != -1)
	{
		m_cache_cycles = cacheCycles;
	}
}

template<class T>
void CachedValue<T>::disableCaching()
{
	m_is_caching = false;
}

template<class T>
bool CachedValue<T>::hasNewValue()
{
	return m_has_been_set && (m_has_new_value || !m_is_caching);
}

template<class T>
bool CachedValue<T>::hasValue()
{
	return m_has_been_set;
}

template<class T>
bool CachedValue<T>::isCaching()
{
	return m_is_caching;
}
///
#endif
