#pragma once
#include "../objects/ScriptObject.h"

namespace script
{
	class ConsoleObject : public ScriptObject
	{
	public:
		ConsoleObject();

		virtual void write(const std::string& value);
		virtual void newline();
		virtual std::string read();
	};
}
