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
