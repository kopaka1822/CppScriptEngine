#pragma once
#include <string>
#include <algorithm>
#include "script/Exception.h"

namespace script
{
	struct L1Token
	{
	public:
		enum class Type
		{
			Undefined,
			String,
			Assign,
			Separator,
			BracketOpen,
			BracketClosed,
			ArrayOpen,
			ArrayClosed,
			Null,
			Identifier,
			Dot,
			Plus,
			//PlusAssign,
			Minus,
			//MinusAssign,
			Multiply,
			//MultiplyAssign,
			Divide,
			//DivideAssign,
			Float,
			Function,
			Bool,
			Integer,
			IdentifierAssign,
		};
		explicit L1Token(Type type, size_t position, std::string value)
			:
		m_value(move(value)),
		m_type(type),
		m_position(position)
		{}

		const std::string& getValue() const
		{
			return m_value;
		}

		Type getType() const
		{
			return m_type;
		}

		size_t getPosition() const
		{
			return m_position;
		}

		bool onlyNumbers() const
		{
			return std::all_of(m_value.begin(), m_value.end(), [](char c)
			{
				return isdigit(static_cast<unsigned char>(c));
			});
		}

		// all numbers except the last letter (must be at least 1 letter)
		bool onlyNumbersWithF() const
		{
			if (m_value.size() < 2) return false;
			if (m_value.back() != 'f') return false;
			return std::all_of(m_value.begin(), m_value.end() - 1, [](char c)
			{
				return isdigit(static_cast<unsigned char>(c));
			});
		}

		bool startsWithLetter() const
		{
			if (m_value.empty()) return false;
			return isalpha(static_cast<unsigned char>(m_value[0]));
		}

		bool startsWithLowercase() const
		{
			if (m_value.empty()) return false;
			return islower(static_cast<unsigned char>(m_value[0]));
		}

		int getIntValue()const try
		{
			return std::stoi(m_value);
		}
		catch (const std::exception&)
		{
			throw ParseError(m_position,"cannot convert " + m_value + " to int");
		}

		float getFloatValue() const try
		{
			return std::stof(m_value);
		}
		catch (const std::exception&)
		{
			throw ParseError(m_position, "cannot convert " + m_value + " to float");
		}

		bool getBoolValue() const
		{
			if (m_value == "true")
				return true;
			if (m_value == "false")
				return false;
			throw ParseError(m_position, "cannot convert " + m_value + " to bool");
		}
	private:
		std::string m_value;
		Type m_type;
		size_t m_position;
	};
}
