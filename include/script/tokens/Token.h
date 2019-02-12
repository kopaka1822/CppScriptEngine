#pragma once
#include <string>

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
		};
		explicit L1Token(Type type, std::string value = "")
			:
		m_value(move(value)),
		m_type(type)
		{}

		const std::string& getValue() const
		{
			return m_value;
		}

		Type getType() const
		{
			return m_type;
		}
	private:
		std::string m_value;
		Type m_type;
	};
}
