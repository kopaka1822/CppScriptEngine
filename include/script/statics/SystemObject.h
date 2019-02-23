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

		/// \brief this functions does nothing. it return a NullObject
		/// you can use this if you don't want to output a result on the console
		ScriptObjectPtr silent(const ScriptObjectPtr&) const;
	private:
		std::shared_ptr<ClockObject> m_clock;
		std::shared_ptr<EngineObject> m_engine;
		std::shared_ptr<IOObject> m_io;
	};
}
