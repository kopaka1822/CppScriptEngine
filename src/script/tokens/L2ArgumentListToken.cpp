#include "../../../include/script/tokens/L2ArgumentListToken.h"

script::ScriptObjectPtr script::L2ArgumentListToken::execute(ScriptEngine& engine) const
{
	// make array from arguments
	auto arr = std::make_shared<ArrayObject>();
	for (const auto& m_value : m_values)
		arr->add(m_value->execute(engine));

	return arr;
}

void script::L2ArgumentListToken::add(std::unique_ptr<L2Token> value)
{
	m_values.emplace_back(move(value));
}