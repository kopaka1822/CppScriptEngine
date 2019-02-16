#include "../include/script/ScriptEngine.h"
#include "script/Tokenizer.h"
#include <iostream>

void script::ScriptEngine::execute(const std::string& command)
{
	if(command.empty()) return;

	std::unique_ptr<L2Token> token;
	try
	{
		token = Tokenizer::getExecutable(command);
		auto res = token->execute(*this);
		std::cout << ">> " << res->toString() << std::endl;
	}
	catch (const ParseError& error)
	{
		auto pos = error.position;
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
		std::cout << "script: " << error.what() << " at " + errorPosition << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cout << "script: " << e.what() << std::endl;
	}
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

	if (name.empty())
		throw std::runtime_error("ScriptEngine::setObject object name was empty");

	if (!islower(static_cast<unsigned char>(name[0])))
		throw std::runtime_error("ScriptEngine::setObject object name must start with a lowercase letter");

	m_objects[name] = object;
}

script::ScriptObjectPtr script::ScriptEngine::getStaticObject(const std::string& object) const
{
	const auto it = m_staticObjects.find(object);
	if (it == m_staticObjects.end()) return nullptr;

	return it->second;
}

void script::ScriptEngine::setStaticObject(const std::string& name, ScriptObjectPtr object)
{
	if (name.empty())
		throw std::runtime_error("ScriptEngine::setStaticObject object name was empty");

	if (!isupper(static_cast<unsigned char>(name[0])))
		throw std::runtime_error("ScriptEngine::setStaticObject object name must start with an uppercase letter");

	if (!object)
		throw std::runtime_error("ScriptEngine::setStaticObject object was null");

	if (m_staticObjects.find(name) != m_staticObjects.end())
		throw std::runtime_error("ScriptEngine::setStaticObject object \"" + name + "\" already exists");

	m_staticObjects[name] = object;
}
