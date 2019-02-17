#include "../../../include/script/tokens/L2IdentifierToken.h"

script::L2IdentifierToken::L2IdentifierToken(std::string name, size_t position)
	:
	m_name(std::move(name)),
	m_position(position)
{
}

script::ScriptObjectPtr script::L2IdentifierToken::execute(ScriptEngine& engine) const
{
	auto obj = engine.getObject(m_name);
	if (!obj)
		throw IdentifierError(m_position, m_name);

	return obj;
}