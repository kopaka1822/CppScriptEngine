#include "../../../include/script/tokens/L2FunctionToken.h"

script::L2FunctionToken::L2FunctionToken(std::unique_ptr<L2Token> value, std::string name, size_t position,
	std::unique_ptr<L2Token> args) :
	m_value(move(value)),
	m_position(position),
	m_funcName(move(name)),
	m_args(move(args))
{
}

script::ScriptObjectPtr script::L2FunctionToken::execute(ScriptEngine& engine) const
{
	auto args = std::dynamic_pointer_cast<ArrayObject>(m_args->execute(engine));
	auto obj = m_value->execute(engine);

	// call function on value
	try
	{
		obj = obj->invoke(m_funcName, args);
	}
	catch (const InvalidFunctionName& e)
	{
		// add line number information
		throw ParseError(m_position, e.what());
	}
	catch (const std::exception& e)
	{
		// add line number information
		throw ParseError(m_position, m_funcName + " exception: " + std::string(e.what()));
	}

	return obj;
}