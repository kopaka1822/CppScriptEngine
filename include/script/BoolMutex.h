#pragma once

namespace script
{
	/// \brief adapter for the std::lock_guard that ensures that a bool is set to false after a function ends
	class BoolMutex
	{
	public:
		void lock()
		{
			m_value = true;
		}
		void unlock()
		{
			m_value = false;
		}
		bool locked() const
		{
			return m_value;
		}
	private:
		bool m_value = false;
	};
}