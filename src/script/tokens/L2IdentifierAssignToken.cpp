#include "../../../include/script/tokens/L2IdentifierAssignToken.h"

script::L2IdentifierAssignToken::L2IdentifierAssignToken(std::string name, std::unique_ptr<L2Token> value) :
	m_name(move(name)),
	m_value(move(value))
{
}

script::ScriptObjectPtr script::L2IdentifierAssignToken::execute(ScriptEngine& engine) const
{
	auto obj = m_value->execute(engine);
	engine.setObject(m_name, obj);
	return obj;
}