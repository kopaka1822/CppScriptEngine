#include "../../../include/script/tokens/L1FunctionRule.h"

int script::L1FunctionRule::patternLength() const
{
	return 2;
}

bool script::L1FunctionRule::matches(std::vector<L1Token>::const_iterator begin) const
{
	if (!(begin->getType() == L1Token::Type::Identifier && (begin + 1)->getType() == L1Token::Type::BracketOpen))
		return false;

	if (!(begin->startsWithLowercase()))
		return false; // functions start with lowercase

	return true;
}

script::L1Token script::L1FunctionRule::apply(std::vector<L1Token>::const_iterator begin) const
{
	return L1Token(L1Token::Type::Function, begin->getPosition(), begin->getValue());
}