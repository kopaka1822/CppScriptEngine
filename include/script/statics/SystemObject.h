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
		SystemObject();

		/// \brief this functions does nothing. it return a NullObject
		/// you can use this if you don't want to output a result on the console
		ScriptObjectPtr silent(const ScriptObjectPtr&) const;
	};
}
