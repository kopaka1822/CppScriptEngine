#include "../../../include/script/tokens/L1FloatRule2.h"

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
	return L1Token(L1Token::Type::Float, begin->getPosition(), move(val));
}
