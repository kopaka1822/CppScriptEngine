#include "../include/script/ScriptEngine.h"

void script::ScriptEngine::addStatic(const std::string& name, ScriptObjectPtr object)
{
	if (name.empty())
		throw std::runtime_error("ScriptEngine::addStatic name was empty");

	m_objects[name] = std::move(object);
}