#include "../include/script/ScriptEngine.h"
#include "../include/script/Tokenizer.h"
#include "../../include/script/objects/FloatObject.h"
#include "../../include/script/statics/ConsoleObject.h"
#include "../../include/script/statics/SystemObject.h"
#include <unordered_set>
#include <cassert>

script::ScriptEngine::ScriptEngine(InitFlags flags)
{
	if(flags & PrimitiveConstructor)
	{
		setStaticFunction("Int", IntObject::getCtor());
		setStaticFunction("Float", FloatObject::getCtor());
		setStaticFunction("Bool", BoolObject::getCtor());
		setStaticFunction("String", StringObject::getCtor());
	}

	if(flags & ConsoleClass)
	{
		setStaticObject("Console", std::make_shared<ConsoleObject>());
	}
	if(flags & SystemClass)
	{
		setStaticObject("System", std::make_shared<SystemObject>());
	}
	if(flags & ClockClass)
	{
		setStaticObject("Clock", std::make_shared<ClockObject>());
	}
	if(flags & IOClass)
	{
		setStaticObject("IO", std::make_shared<IOObject>(*this));
	}
	if(flags & EngineClass)
	{
		setStaticObject("Engine", std::make_shared<EngineObject>(*this));
	}
}

script::ScriptObjectPtr script::ScriptEngine::execute(const std::string& command)
{
	if(command.empty()) return NullObject::get();

	try
	{
		const auto token = Tokenizer::getExecutable(command);
		const auto res = token->execute(*this);
		return res;
	}
	catch (const ParseError& error)
	{
		const auto pos = error.position;
		std::string errorPosition;
		if(pos == size_t(-1))
		{
			errorPosition = command + "<";
		}
		else
		{
			// include some parts before and some parts after the error
			errorPosition = command;
			errorPosition.insert(pos, ">");
		}
		throw std::runtime_error(error.what() + std::string(" at ") + errorPosition);
	}
}

script::ScriptObjectPtr script::ScriptEngine::getObject(const std::string& object) const
{
	const auto it = m_objects.find(object);
	if (it == m_objects.end()) return nullptr;

	return it->second;
}

void script::ScriptEngine::setObject(const std::string& name, const ScriptObjectPtr& object)
{
	if(!object)
	{
		m_objects.erase(name);
		return;
	}

	if (name.empty())
		throw std::runtime_error("ScriptEngine::setObject object name was empty");

	if (!islower(static_cast<unsigned char>(name[0])))
		throw std::runtime_error("ScriptEngine::setObject object name must start with a lowercase letter");

	m_objects[name] = object;
}

void script::ScriptEngine::removeObjectVariables(const ScriptObjectPtr& object)
{
	auto it = m_objects.begin();
	while (it != m_objects.end())
	{
		if(it->second.get() == object.get())
			it = m_objects.erase(it);
		else
			++it;
	}
}

void script::ScriptEngine::clearObjects()
{
	m_objects.clear();
}

script::ScriptObjectPtr script::ScriptEngine::getStaticObject(const std::string& object) const
{
	const auto it = m_staticObjects.find(object);
	if (it == m_staticObjects.end()) return nullptr;

	return it->second;
}

void script::ScriptEngine::setStaticObject(const std::string& name, const ScriptObjectPtr& object, bool mayOverwrite)
{
	if (name.empty())
		throw std::runtime_error("ScriptEngine::setStaticObject object name was empty");

	if (!isupper(static_cast<unsigned char>(name[0])))
		throw std::runtime_error("ScriptEngine::setStaticObject object name must start with an uppercase letter");

	if (!object)
		throw std::runtime_error("ScriptEngine::setStaticObject object was null");

	if (!mayOverwrite && m_staticObjects.find(name) != m_staticObjects.end())
		throw std::runtime_error("ScriptEngine::setStaticObject object \"" + name + "\" already exists");

	m_staticObjects[name] = object;
}

void script::ScriptEngine::setStaticFunction(const std::string& name, const ScriptObject::FunctionT& function)
{
	if (name.empty())
		throw std::runtime_error("ScriptEngine::setStaticFunction function name was empty");

	if (!isupper(static_cast<unsigned char>(name[0])))
		throw std::runtime_error("ScriptEngine::setStaticFunction function name must start with an uppercase letter");

	if (!function)
		throw std::runtime_error("ScriptEngine::setStaticFunction function was null");

	if (m_staticFunctions.find(name) != m_staticFunctions.end())
		throw std::runtime_error("ScriptEngine::setStaticFunction function \"" + name + "\" already exists");

	m_staticFunctions[name] = function;
}

std::vector<std::string> script::ScriptEngine::getAutocomplete(const std::string& text)
{
	std::vector<std::string> res;
	std::unordered_set<std::string> candidates;

	const auto info = Tokenizer::getAutocomplete(text);
	
	if (info.noProposal)
		return res; // no proposals available

	if (info.objectToken.getType() == L1Token::Type::OpenString)
		return res; // no proposals for now

	bool hasPreviousElement = true;

	// get possible functions
	if (info.callerObject)
	{
		addAllFunctions(candidates, *info.callerObject);
	}
	else if(info.callerToken.getType() == L1Token::Type::Identifier)
	{
		// find the identifier
		if(info.callerToken.startsWithLowercase())
		{
			// variable
			const auto it = m_objects.find(info.callerToken.getValue());
			if (it != m_objects.end())
				addAllFunctions(candidates, *it->second);
			else // add default functions
				addAllFunctions(candidates, *NullObject::get());
		}
		else if(info.callerToken.startWithUppercase())
		{
			// static object
			const auto it = m_staticObjects.find(info.callerToken.getValue());
			if (it != m_staticObjects.end())
				addAllFunctions(candidates, *it->second);
			else // add default functions
				addAllFunctions(candidates, *NullObject::get());
		}
	} 
	else
	{
		// no prior element => could be any object or static function
		hasPreviousElement = false;
		for (const auto& v : m_objects)
			candidates.insert(v.first);
		for (const auto& v : m_staticObjects)
			candidates.insert(v.first);
		for (const auto& v : m_staticFunctions)
			candidates.insert(v.first + "(");
	}

	// match value with candidates
	if(info.objectToken.getType() == L1Token::Type::Undefined)
	{
		// everything is possible
		res.reserve(candidates.size());
		for (auto& v : candidates)
			res.push_back(v);

		// prefer getter setter over normal functions, prefer variables over static objects
		const auto mid = std::partition(res.begin(), res.end(), [hasPreviousElement](const std::string& value)
		{
			// starts with uppercase?
			const bool upper = isupper(value.front()) != 0;
			// is function type?
			if (hasPreviousElement)
				return upper; // yes => prefer getter, setter
			return !upper; // no => prefer variables
		});

		// sort partitions
		std::sort(res.begin(), mid);
		std::sort(mid, res.end());
	}
	else
	{
		assert(info.objectToken.getType() == L1Token::Type::Identifier);
		// only use candidates that start with the value
		const auto prefix = info.objectToken.getValue();
		for (auto& c : candidates)
			// start with prefix and are longer than prefix
			if (c.rfind(prefix, 0) == 0 && c.size() > prefix.size())
				res.push_back(c.substr(prefix.size()));

		// sort functions
		std::sort(res.begin(), res.end());
	}

	return res;
}

void script::ScriptEngine::addAllFunctions(std::unordered_set<std::string>& set, const ScriptObject& obj)
{
	for (const auto& v : obj.getFunctions())
		set.insert(v + "(");

	for (const auto& v : obj.getGetter())
		set.insert(v);

	for (const auto& v : obj.getSetter())
		set.insert(v);
}

script::ScriptObject::FunctionT script::ScriptEngine::getStaticFunction(const std::string& name)
{
	const auto it = m_staticFunctions.find(name);
	if (it == m_staticFunctions.end()) return nullptr;

	return it->second;
}
