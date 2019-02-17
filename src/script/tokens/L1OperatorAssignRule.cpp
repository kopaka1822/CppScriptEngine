#include "../../../include/script/tokens/L1OperatorAssignRule.h"

int script::L1OperatorAssignRule::patternLength() const
{
	return 2;
}

bool script::L1OperatorAssignRule::matches(std::vector<L1Token>::const_iterator begin) const
{
	switch (begin->getType())
	{
	case L1Token::Type::Plus:
	case L1Token::Type::Minus:
	case L1Token::Type::Multiply:
	case L1Token::Type::Divide:
		break;
	default: return false;
	}

	return (begin + 1)->getType() == L1Token::Type::Assign;
}

script::L1Token script::L1OperatorAssignRule::apply(std::vector<L1Token>::const_iterator begin) const
{
	switch (begin->getType())
	{
	case L1Token::Type::Plus:
		return L1Token(L1Token::Type::PlusAssign, begin->getPosition(), "+=");
	case L1Token::Type::Minus:
		return L1Token(L1Token::Type::MinusAssign, begin->getPosition(), "-=");
	case L1Token::Type::Multiply:
		return L1Token(L1Token::Type::MultiplyAssign, begin->getPosition(), "*=");
	case L1Token::Type::Divide:
		return L1Token(L1Token::Type::DivideAssign, begin->getPosition(), "/=");
	}
	return L1Token(L1Token::Type::Undefined, begin->getPosition(), begin->getValue());
}