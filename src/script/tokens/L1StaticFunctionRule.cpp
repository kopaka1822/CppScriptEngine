#include "../../../include/script/tokens/L1StaticFunctionRule.h"

int script::L1StaticFunctionRule::patternLength() const
{
	return 2;
}

bool script::L1StaticFunctionRule::matches(std::vector<L1Token>::const_iterator begin) const
{
	if (!(begin->getType() == L1Token::Type::Identifier && (begin + 1)->getType() == L1Token::Type::BracketOpen))
		return false;

	if (!(begin->startWithUppercase()))
		return false; // static functions start with uppercase

	return true;
}

script::L1Token script::L1StaticFunctionRule::apply(std::vector<L1Token>::const_iterator begin) const
{
	return L1Token(L1Token::Type::StaticFunction, begin->getPosition(), begin->getValue());
}
