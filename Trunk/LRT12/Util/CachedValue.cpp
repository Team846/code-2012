#include "CachedValue.h"

CachedInt::CachedInt(int initialValue, int cacheCycles)
{
	m_value = initialValue;
	m_cache_cycles = cacheCycles;
	enableCaching(cacheCycles);
	uncache();
	m_has_been_set = true;
}

CachedInt::CachedInt()
{
	enableCaching(25);
	m_has_new_value = false;
	m_has_been_set = false;
}

void CachedInt::setValue(int newValue)
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

int CachedInt::getValue()
{
	m_has_new_value = false;
	m_previous_value = m_value;
	return m_value;
}

int CachedInt::peek()
{
	return m_value;
}

void CachedInt::uncache()
{
	m_has_new_value = true;
	m_counter = 0;
}

void CachedInt::incrementCounter()
{
	if (!m_is_caching || ++m_counter >= m_cache_cycles)
	{
		uncache();
	}
}

void CachedInt::enableCaching(int cacheCycles)
{
	m_is_caching = true;

	if (cacheCycles != -1)
	{
		m_cache_cycles = cacheCycles;
	}
}

void CachedInt::disableCaching()
{
	m_is_caching = false;
}

bool CachedInt::hasNewValue()
{
	return m_has_been_set && (m_has_new_value || !m_is_caching);
}

bool CachedInt::isCaching()
{
	return m_is_caching;
}

CachedFloat::CachedFloat(float initialValue, int cacheCycles)
{
	m_value = initialValue;
	m_cache_cycles = cacheCycles;
	enableCaching(cacheCycles);
	uncache();
	m_has_been_set = true;
}

CachedFloat::CachedFloat()
{
	enableCaching(25);
	m_has_new_value = false;
	m_has_been_set = false;
}

void CachedFloat::setValue(float newValue)
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

float CachedFloat::getValue()
{
	m_has_new_value = false;
	m_previous_value = m_value;
	return m_value;
}

float CachedFloat::peek()
{
	return m_value;
}

void CachedFloat::uncache()
{
	m_has_new_value = true;
	m_counter = 0;
}

void CachedFloat::incrementCounter()
{
	if (!m_is_caching || ++m_counter >= m_cache_cycles)
	{
		uncache();
	}
}

void CachedFloat::enableCaching(int cacheCycles)
{
	m_is_caching = true;

	if (cacheCycles != -1)
	{
		m_cache_cycles = cacheCycles;
	}
}

void CachedFloat::disableCaching()
{
	m_is_caching = false;
}

bool CachedFloat::hasNewValue()
{
	return m_has_been_set && (m_has_new_value || !m_is_caching);
}

bool CachedFloat::isCaching()
{
	return m_is_caching;
}

