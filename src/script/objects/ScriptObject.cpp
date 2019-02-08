#include "../../../include/script/objects/ScriptObject.h"
#include "../../../include/script/objects/ScriptObjectArray.h"
#include "../../../include/script/objects/StringObject.h"
#include "../../../include/script/Util.h"

script::ScriptObject::ScriptObject()
{
	addFunction("clone", [this](ScriptObjectArrayPtr args)
	{
		if (args->count() != 0)
			throw std::runtime_error("invalid argument count ScriptObject::clone()");

		return this->clone();
	});
	addFunction("toString", [this](ScriptObjectArrayPtr args)
	{
		if (args->count() != 0)
			throw std::runtime_error("invalid argument count ScriptObject::toString()");

		return std::make_shared<StringObject>(toString());
	});
}

script::ScriptObjectPtr script::ScriptObject::invoke(const std::string& funcName,
	const ScriptPtr<ScriptObjectArray>& args)
{
	const auto it = m_functions.find(funcName);
	if (it == m_functions.end())
		throw std::runtime_error("ScriptObject::invoke function " + funcName + " not found");
	
	return it->second(args);
}

script::ScriptObjectPtr script::ScriptObject::getMember(const std::string& memName)
{
	const auto it = m_members.find(memName);
	if(it == m_members.end())
		throw std::runtime_error("ScriptObject::getMember member " + memName + " not found");

	return it->second;
}

void script::ScriptObject::addFunction(const std::string& name, const FunctionT& func)
{
	// elements already inside?
	if (m_functions.find(name) != m_functions.end())
		throw std::runtime_error("ScriptObject::addFunction name already used: " + name);

	m_functions[name] = func;
}

void script::ScriptObject::addMember(const std::string& name, ScriptObjectPtr member)
{
	// elements already inside?
	if (m_members.find(name) != m_members.end())
		throw std::runtime_error("ScriptObject::addMember name already used: " + name);

	m_members[name] = std::move(member);
}