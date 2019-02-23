#pragma once

namespace script
{
	/// \brief adapter for the std::lock_guard that ensures that an int is decremented after a function ends
	class IntMutex
	{
	public:
		void lock()
		{
			++m_value;
		}
		void unlock()
		{
			--m_value;
		}
		int getValue() const
		{
			return m_value;
		}
	private:
		int m_value = 0;
	};
}