#include "../../../include/script/tokens/L1IdentifierAssignRule.h"

int script::L1IdentifierAssignRule::patternLength() const
{
	return 2;
}

bool script::L1IdentifierAssignRule::matches(std::vector<L1Token>::const_iterator begin) const
{
	return begin->getType() == L1Token::Type::Identifier && (begin + 1)->getType() == L1Token::Type::Assign;
}

script::L1Token script::L1IdentifierAssignRule::apply(std::vector<L1Token>::const_iterator begin) const
{
	return L1Token(L1Token::Type::IdentifierAssign, begin->getPosition(), begin->getValue());
}