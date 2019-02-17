#include "../../../include/script/tokens/L1NumberRule.h"
#include "../../../include/script/Exception.h"

int script::L1NumberRule::patternLength() const
{
	return 1;
}

bool script::L1NumberRule::matches(std::vector<L1Token>::const_iterator begin) const
{
	return begin->getType() == L1Token::Type::Identifier;
}

script::L1Token script::L1NumberRule::apply(std::vector<L1Token>::const_iterator begin) const
{
	// determine identifier type
	if (begin->getValue() == "true") return L1Token(L1Token::Type::Bool, begin->getPosition(), "true");
	if (begin->getValue() == "false") return L1Token(L1Token::Type::Bool, begin->getPosition(), "false");
	if (begin->getValue() == "null") return L1Token(L1Token::Type::Null, begin->getPosition(), "null");

	if (begin->onlyNumbers()) return L1Token(L1Token::Type::Integer, begin->getPosition(), begin->getValue());
	if (begin->onlyNumbersWithF()) return L1Token(L1Token::Type::Float, begin->getPosition(), begin->getValue());

	// is it a valid identifier?
	if (!(begin->startsWithLetter()))
		throw SyntaxError(begin->getPosition(), begin->getValue(), "is not valid number");

	return *begin; // leave as is
}