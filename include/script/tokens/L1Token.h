#pragma once
#include <string>
#include <algorithm>

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
			Minus,
			Multiply,
			Divide,
			Float,
			Function,
			Bool,
			Integer,
		};
		explicit L1Token(Type type, size_t position, std::string value = "")
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
	private:
		std::string m_value;
		Type m_type;
		size_t m_position;
	};
}
