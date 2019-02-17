#include "../../../include/script/tokens/L2StaticIdentifierToken.h"

script::L2StaticIdentifierToken::L2StaticIdentifierToken(std::string name, size_t position)
	:
	m_name(std::move(name)),
	m_position(position)
{
}

script::ScriptObjectPtr script::L2StaticIdentifierToken::execute(ScriptEngine& engine) const
{
	auto obj = engine.getStaticObject(m_name);
	if (!obj)
		throw IdentifierError(m_position, m_name);

	return obj;
}