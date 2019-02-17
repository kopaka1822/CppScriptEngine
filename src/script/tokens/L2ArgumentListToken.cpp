#include "../../../include/script/tokens/L2ArgumentListToken.h"

script::ScriptObjectPtr script::L2ArgumentListToken::execute(ScriptEngine& engine) const
{
	// make array from arguments
	auto arr = std::make_shared<ArrayObject>();
	for (auto i = m_values.begin(), end = m_values.end(); i != end; ++i)
		arr->add((*i)->execute(engine));

	return arr;
}

void script::L2ArgumentListToken::add(std::unique_ptr<L2Token> value)
{
	m_values.emplace_back(move(value));
}