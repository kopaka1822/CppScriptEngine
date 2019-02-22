#pragma once
#include "../ScriptEngine.h"
#include "ClockObject.h"
#include "EngineObject.h"

namespace script
{
	class SystemObject : public ScriptObject
	{
	public:
		SystemObject(ScriptEngine &engine);
		std::shared_ptr<ClockObject> getClock() const;
		std::shared_ptr<EngineObject> getEngine() const;
	private:
		std::shared_ptr<ClockObject> m_clock;
		std::shared_ptr<EngineObject> m_engine;
	};
}
