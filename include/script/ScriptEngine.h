#pragma once
#include <string>
#include <unordered_map>
#include "objects/ScriptObject.h"
#include "objects/ArrayObject.h"
#include "objects/IntObject.h"
#include "objects/StringObject.h"
#include "objects/NullObject.h"
#include "objects/BoolObject.h"
#include "Util.h"

namespace script
{
	class ScriptEngine
	{
	public:
		void execute(const std::string& command);
		void addStatic(const std::string& name, ScriptObjectPtr object);

	private:
		std::unordered_map<std::string, ScriptObjectPtr> m_objects;
	};
}