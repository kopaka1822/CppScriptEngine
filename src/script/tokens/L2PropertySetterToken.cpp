#include "../../../include/script/tokens/L2PropertySetterToken.h"

script::L2PropertySetterToken::L2PropertySetterToken(std::unique_ptr<L2Token> object, std::unique_ptr<L2Token> arg,
	std::string propName, size_t position) :
	m_object(move(object)),
	m_arg(move(arg)),
	m_funcName(move(propName)),
	m_position(position)
{
}

script::ScriptObjectPtr script::L2PropertySetterToken::execute(ScriptEngine& engine) const
{
	const auto arg = m_object->execute(engine);
	auto obj = m_object->execute(engine);
	try
	{
		obj = obj->invoke("set" + m_funcName, Util::makeArray(arg));
	}
	catch (const InvalidFunctionName&)
	{
		// add line number information
		throw ParseError(m_position, "property setter " + m_funcName + " not found");
	}
	catch (const std::exception& e)
	{
		// add line number information
		throw ParseError(m_position, "set" + m_funcName + " exception: " + std::string(e.what()));
	}
	return obj;
}