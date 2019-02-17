#include "../../../include/script/tokens/L1Token.h"
#include "../../../include/script/Exception.h"
#include <algorithm>

script::L1Token::L1Token(Type type, size_t position, std::string value)
	:
	m_value(move(value)),
	m_type(type),
	m_position(position)
{}

const std::string& script::L1Token::getValue() const
{
	return m_value;
}

script::L1Token::Type script::L1Token::getType() const
{
	return m_type;
}

size_t script::L1Token::getPosition() const
{
	return m_position;
}

bool script::L1Token::onlyNumbers() const
{
	return std::all_of(m_value.begin(), m_value.end(), [](char c)
	{
		return isdigit(static_cast<unsigned char>(c));
	});
}

bool script::L1Token::onlyNumbersWithF() const
{
	if (m_value.size() < 2) return false;
	if (m_value.back() != 'f') return false;
	return std::all_of(m_value.begin(), m_value.end() - 1, [](char c)
	{
		return isdigit(static_cast<unsigned char>(c));
	});
}

bool script::L1Token::startsWithLetter() const
{
	if (m_value.empty()) return false;
	return isalpha(static_cast<unsigned char>(m_value[0]));
}

bool script::L1Token::startsWithLowercase() const
{
	if (m_value.empty()) return false;
	return islower(static_cast<unsigned char>(m_value[0]));
}

bool script::L1Token::startWithUppercase() const
{
	if (m_value.empty()) return false;
	return isupper(static_cast<unsigned char>(m_value[0]));
}

int script::L1Token::getIntValue() const
try
{
	return std::stoi(m_value);
}
catch (const std::exception&)
{
	throw ParseError(m_position, "cannot convert " + m_value + " to int");
}

float script::L1Token::getFloatValue() const
try
{
	return std::stof(m_value);
}
catch (const std::exception&)
{
	throw ParseError(m_position, "cannot convert " + m_value + " to float");
}

bool script::L1Token::getBoolValue() const
{
	if (m_value == "true")
		return true;
	if (m_value == "false")
		return false;
	throw ParseError(m_position, "cannot convert " + m_value + " to bool");
}
