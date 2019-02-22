#pragma once
#include "../ScriptEngine.h"
#include "ClockObject.h"
#include "EngineObject.h"
#include "IOObject.h"

namespace script
{
	class SystemObject : public ScriptObject
	{
	public:
		SystemObject(ScriptEngine &engine);
		std::shared_ptr<ClockObject> getClock() const;
		std::shared_ptr<EngineObject> getEngine() const;
		std::shared_ptr<IOObject> getIO() const;
	private:
		std::shared_ptr<ClockObject> m_clock;
		std::shared_ptr<EngineObject> m_engine;
		std::shared_ptr<IOObject> m_io;
	};
}
