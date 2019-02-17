#pragma once
#include <string>

namespace script
{
	struct L1Token
	{
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
			PlusAssign,
			Minus,
			MinusAssign,
			Multiply,
			MultiplyAssign,
			Divide,
			DivideAssign,
			Float,
			Function,
			Bool,
			Integer,
			IdentifierAssign,
		};

		explicit L1Token(Type type, size_t position, std::string value);

		const std::string& getValue() const;
		Type getType() const;
		size_t getPosition() const;

		bool onlyNumbers() const;
		// all numbers except the last letter (must be at least 1 letter)
		bool onlyNumbersWithF() const;

		bool startsWithLetter() const;
		bool startsWithLowercase() const;
		bool startWithUppercase() const;

		int getIntValue() const;
		float getFloatValue() const;
		bool getBoolValue() const;

	private:
		std::string m_value;
		Type m_type;
		size_t m_position;
	};
}
