#pragma once
#include <string>
#include "../ScriptEngine.h"

namespace script
{
	class L2Token
	{
	public:
		virtual ~L2Token() = default;
		virtual ScriptObjectPtr execute(ScriptEngine& engine) const = 0;
	};
}
