#pragma once
#include "../ScriptEngine.h"
#include "ClockObject.h"

namespace script
{
	class SystemObject : public ScriptObject
	{
	public:
		SystemObject();
		std::shared_ptr<ClockObject> getClock() const;

	private:
		std::shared_ptr<ClockObject> m_clock;
	};
}
