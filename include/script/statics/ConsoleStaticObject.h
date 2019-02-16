#pragma once
#include "../objects/ScriptObject.h"

namespace script
{
	class ConsoleStaticObject : public ScriptObject
	{
	public:
		ConsoleStaticObject();

		virtual void write(const std::string& value);
		virtual void newline();
		virtual std::string read();
	};
}
