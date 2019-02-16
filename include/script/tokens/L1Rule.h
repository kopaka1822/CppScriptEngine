#pragma once
#include <vector>
#include "L1Token.h"
#include "../Exception.h"

namespace script
{
	class L1Rule
	{
	public:
		virtual ~L1Rule() = default;
		/// \brief indicates the amount of tokens that should be considered
		virtual int patternLength() const = 0;
		/// \brief indicates if the sequence starting with begin matches the pattern
		virtual bool matches(std::vector<L1Token>::const_iterator begin) const = 0;
		/// \brief applies pattern to the sequence and returns the new sequence
		virtual L1Token apply(std::vector<L1Token>::const_iterator begin) const = 0;
	};

	/// \brief for floats with a dot: 3.4f 1.f 1224.2434f
	class L1FloatRule1 final : public L1Rule
	{
	public:
		int patternLength() const override
		{
			return 3;
		}

		bool matches(std::vector<L1Token>::const_iterator begin) const override
		{
			if(!(begin->getType() == L1Token::Type::Identifier &&
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

		L1Token apply(std::vector<L1Token>::const_iterator begin) const override
		{
			auto val = begin->getValue() + "." + (begin + 2)->getValue();
			return L1Token(L1Token::Type::Float, begin->getPosition(), val);
		}
	};

	/// \brief rule for .5f .25f etc. (floats starting with a point)
	class L1FloatRule2 final : public L1Rule
	{
	public:
		int patternLength() const override
		{
			return 2;
		}

		bool matches(std::vector<L1Token>::const_iterator begin) const override
		{
			if (!(begin->getType() == L1Token::Type::Dot &&
				(begin + 1)->getType() == L1Token::Type::Identifier))
				return false;

			// second block ends with f
			return (begin + 1)->onlyNumbersWithF();
		}

		L1Token apply(std::vector<L1Token>::const_iterator begin) const override
		{
			auto val = "0." + (begin + 1)->getValue();
			return L1Token(L1Token::Type::Float, begin->getPosition(), val);
		}
	};

	/// \brief for int, float, bool, null or identifier (without a dot)
	class L1NumberRule final : public L1Rule
	{
	public:
		int patternLength() const override { return 1; }
		
		bool matches(std::vector<L1Token>::const_iterator begin) const override
		{
			return begin->getType() == L1Token::Type::Identifier;
		}

		L1Token apply(std::vector<L1Token>::const_iterator begin) const override
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
	};

	class L1FunctionRule final : public L1Rule
	{
	public:
		int patternLength() const override { return 2; }
		
		bool matches(std::vector<L1Token>::const_iterator begin) const override
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

		L1Token apply(std::vector<L1Token>::const_iterator begin) const override
		{
			return L1Token(L1Token::Type::Function, begin->getPosition(), begin->getValue());
		}
	};

	// converts + (Plus) and = (assign) into += PlusAssign (same for - * /)
	//class L1OperatorAssignRule final : public L1Rule
	//{
	//public:
	//	int patternLength() const override { return 2; }
	//
	//	bool matches(std::vector<L1Token>::const_iterator begin) const override
	//	{
	//		switch (begin->getType())
	//		{
	//		case L1Token::Type::Plus:
	//		case L1Token::Type::Minus:
	//		case L1Token::Type::Multiply:
	//		case L1Token::Type::Divide:
	//			break;
	//		default: return false;
	//		}
	//
	//		return (begin + 1)->getType() == L1Token::Type::Assign;
	//	}
	//	
	//	L1Token apply(std::vector<L1Token>::const_iterator begin) const override
	//	{
	//		switch (begin->getType())
	//		{
	//		case L1Token::Type::Plus:
	//			return L1Token(L1Token::Type::PlusAssign, begin->getPosition());
	//		case L1Token::Type::Minus:
	//			return L1Token(L1Token::Type::MinusAssign, begin->getPosition());
	//		case L1Token::Type::Multiply:
	//			return L1Token(L1Token::Type::MultiplyAssign, begin->getPosition());
	//		case L1Token::Type::Divide:
	//			return L1Token(L1Token::Type::DivideAssign, begin->getPosition());
	//		}
	//		return L1Token(L1Token::Type::Undefined, begin->getPosition());
	//	}
	//};

	class L1IdentifierAssignRule final : public L1Rule
	{
	public:
		int patternLength() const override {return 2; }

		bool matches(std::vector<L1Token>::const_iterator begin) const override
		{
			return begin->getType() == L1Token::Type::Identifier && (begin + 1)->getType() == L1Token::Type::Assign;
		}

		L1Token apply(std::vector<L1Token>::const_iterator begin) const override
		{
			return L1Token(L1Token::Type::IdentifierAssign, begin->getPosition(), begin->getValue());
		}
	};
}
