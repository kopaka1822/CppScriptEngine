#include "../../../include/script/tokens/L2PropertyGetterToken.h"

script::L2PropertyGetterToken::L2PropertyGetterToken(std::unique_ptr<L2Token> object, std::string propName,
	size_t position) :
	m_object(move(object)),
	m_funcName(move(propName)),
	m_position(position)
{
}

script::ScriptObjectPtr script::L2PropertyGetterToken::execute(ScriptEngine& engine) const
{
	auto obj = m_object->execute(engine);
	try
	{
		obj = obj->invoke("get" + m_funcName, Util::makeArray());
	}
	catch (const InvalidFunctionName&)
	{
		// add line number information
		throw ParseError(m_position, "property getter " + m_funcName + " not found");
	}
	catch (const std::exception& e)
	{
		// add line number information
		throw ParseError(m_position, "get" + m_funcName + " exception: " + std::string(e.what()));
	}
	return obj;
}