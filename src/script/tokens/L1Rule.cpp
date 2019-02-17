#include "../../../include/script/tokens/L1Rule.h"
#include "../../../include/script/Exception.h"

int script::L1FloatRule1::patternLength() const
{
	return 3;
}
bool script::L1FloatRule1::matches(std::vector<L1Token>::const_iterator begin) const
{
	if (!(begin->getType() == L1Token::Type::Identifier &&
		(begin + 1)->getType() == L1Token::Type::Dot &&
		(begin + 2)->getType() == L1Token::Type::Identifier))
		return false;
	// test if convertible to float
	// first block only numbers
	if (!(begin->onlyNumbers())) return false;
	// second block ends with f

	// numbers with f or only the f
	return (begin + 2)->onlyNumbersWithF() || (begin->getValue() == "f");
}

script::L1Token script::L1FloatRule1::apply(std::vector<L1Token>::const_iterator begin) const
{
	auto val = begin->getValue() + "." + (begin + 2)->getValue();
	return L1Token(L1Token::Type::Float, begin->getPosition(), val);
}

int script::L1FloatRule2::patternLength() const
{
	return 2;
}

bool script::L1FloatRule2::matches(std::vector<L1Token>::const_iterator begin) const
{
	if (!(begin->getType() == L1Token::Type::Dot &&
		(begin + 1)->getType() == L1Token::Type::Identifier))
		return false;

	// second block ends with f
	return (begin + 1)->onlyNumbersWithF();
}

script::L1Token script::L1FloatRule2::apply(std::vector<L1Token>::const_iterator begin) const
{
	auto val = "0." + (begin + 1)->getValue();
	return L1Token(L1Token::Type::Float, begin->getPosition(), val);
}

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

	// check function syntax
	if (!(begin->startsWithLetter()))
		throw SyntaxError(begin->getPosition(), begin->getValue(), "functions must start with a letter");

	return true;
}

script::L1Token script::L1FunctionRule::apply(std::vector<L1Token>::const_iterator begin) const
{
	return L1Token(L1Token::Type::Function, begin->getPosition(), begin->getValue());
}

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
