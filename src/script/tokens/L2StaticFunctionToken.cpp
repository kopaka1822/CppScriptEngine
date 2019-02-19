#include "../../../include/script/tokens/L2StaticFunctionToken.h"

script::L2StaticFunctionToken::L2StaticFunctionToken(std::string name, size_t position, std::unique_ptr<L2Token> args) 
	:
m_position(position),
m_funcName(move(name)),
m_args(move(args))
{
	
}

script::ScriptObjectPtr script::L2StaticFunctionToken::execute(ScriptEngine& engine) const
{
	// obtain function
	const auto func = engine.getStaticFunction(m_funcName);
	if (!func)
		throw ParseError(m_position, "static function \"" + m_funcName + "\" not found");

	const auto args = std::dynamic_pointer_cast<ArrayObject>(m_args->execute(engine));

	// call function
	try
	{
		return func(args);
	}
	catch (const std::exception& e)
	{
		// add line number information
		throw ParseError(m_position, m_funcName + "(...) exception: " + std::string(e.what()));
	}
}
