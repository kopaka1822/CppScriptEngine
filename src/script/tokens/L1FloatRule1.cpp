#include "../../../include/script/tokens/L1FloatRule1.h"

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
	return (begin + 2)->onlyNumbersWithF() || ((begin + 2)->getValue() == "f")
#ifndef SCRIPT_STRONG_FLOAT_TYPING
	|| (begin + 2)->onlyNumbers()
#endif
	;
}

script::L1Token script::L1FloatRule1::apply(std::vector<L1Token>::const_iterator begin) const
{
	const auto val = begin->getValue() + "." + (begin + 2)->getValue();
	return L1Token(L1Token::Type::Float, begin->getPosition(), val);
}