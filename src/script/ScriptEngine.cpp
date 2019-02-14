#include "../include/script/ScriptEngine.h"

void script::ScriptEngine::addStatic(const std::string& name, ScriptObjectPtr object)
{
	if (name.empty())
		throw std::runtime_error("ScriptEngine::addStatic name was empty");

	m_objects[name] = std::move(object);
}

script::ScriptObjectPtr script::ScriptEngine::getObject(const std::string& object) const
{
	const auto it = m_objects.find(object);
	if (it == m_objects.end()) return nullptr;

	return it->second;
}

void script::ScriptEngine::setObject(const std::string& name, ScriptObjectPtr object)
{
	if(!object)
	{
		m_objects.erase(name);
		return;
	}

	m_objects[name] = object;
}
