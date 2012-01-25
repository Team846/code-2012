#include "CachedValue.h"

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
	enableCaching(25);
	m_has_new_value = false;
	m_has_been_set = false;
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
bool CachedValue<T>::isCaching()
{
	return m_is_caching;
}
