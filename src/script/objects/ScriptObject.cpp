#include "../../../include/script/objects/ScriptObject.h"
#include "../../../include/script/objects/ArrayObject.h"
#include "../../../include/script/Util.h"
#include <cctype>
#include <any>
#include <algorithm>

script::ScriptObject::ScriptObject()
{
	addFunction("clone", Util::makeFunction(this, &ScriptObject::clone, "object ScriptObject::clone()"));
	addFunction("toString", Util::makeFunction(this, &ScriptObject::toString, "string ScriptObject::toString()"));
	addFunction("equals", Util::makeFunction(this, &ScriptObject::equals, "bool ScriptObject::equals(object)"));
	addFunction("invoke", Util::makeFunction(this, &ScriptObject::invoke, "object ScriptObject::invoke(string name, array arguments)"));
	addFunction("getFunctions", Util::makeFunction(this, &ScriptObject::getFunctions, "array ScriptObject::getFunctions()"));
	addFunction("getGetter", Util::makeFunction(this, &ScriptObject::getGetter, "array ScriptObject::getGetter()"));
	addFunction("getSetter", Util::makeFunction(this, &ScriptObject::getSetter, "array ScriptObject::getSetter()"));
}

std::string script::ScriptObject::toString() const
{
	return type();
}

script::ScriptObjectPtr script::ScriptObject::clone() const
{
	throw ObjectNotCloneableException(type());
}

bool script::ScriptObject::equals(const ScriptObjectPtr& other) const
{
	// reference equals
	return this == other.get();
}

std::string script::ScriptObject::type() const
{
	return Util::prettyTypeName(typeid(*this).name());
}

size_t script::ScriptObject::hashCode() const
{
	return typeid(*this).hash_code();
}

script::ScriptObjectPtr script::ScriptObject::invoke(const std::string& funcName, const ArrayObjectPtr& args)
{
	const auto it = m_functions.find(funcName);
	if (it == m_functions.end())
		throw InvalidFunctionName("ScriptObject::invoke", funcName);
	
	return it->second(args);
}

std::vector<std::string> script::ScriptObject::getFunctions() const
{
	std::vector<std::string> res;
	res.reserve(m_functions.size());
	for (const auto& func : m_functions)
		res.push_back(func.first);

	return res;
}

std::vector<std::string> script::ScriptObject::getGetter() const
{
	std::vector<std::string> res;
	res.reserve(m_functions.size());
	for (const auto& func : m_functions)
		// starts with get?
		if(func.first.rfind("get", 0) == 0 
			&& func.first.size() > 3 && isupper(func.first[3]))
			res.push_back(func.first.substr(3));

	return res;
}

std::vector<std::string> script::ScriptObject::getSetter() const
{
	std::vector<std::string> res;
	res.reserve(m_functions.size());
	for (const auto& func : m_functions)
		// starts with set?
		if (func.first.rfind("set", 0) == 0 
			&& func.first.size() > 3 && isupper(func.first[3]))
			res.push_back(func.first.substr(3));

	return res;
}

void script::ScriptObject::addFunction(const std::string& name, const FunctionT& func)
{
	if (name.empty())
		throw std::runtime_error("ScriptObject::addFunction name was empty");
	if (!islower(name[0]))
		throw std::runtime_error("ScriptObject::addFunction name must start with a lowercase letter");
	if(std::any_of(name.begin(), name.end(), isspace))
		throw std::runtime_error("ScriptObject::addFunction name must not contain spaces");

	// elements already inside?
	if (m_functions.find(name) != m_functions.end())
		throw std::runtime_error("ScriptObject::addFunction name already used: " + name);

	m_functions[name] = func;
}

void script::ScriptObject::removeFunction(const std::string& name) noexcept
{
	const auto it = m_functions.find(name);
	if (it == m_functions.end())
		return;

	m_functions.erase(it);
}
