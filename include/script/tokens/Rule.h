#pragma once
#include <vector>
#include "L1Token.h"

namespace script
{
	class L1Rule
	{
	public:
		virtual ~L1Rule() = default;
		/// \brief indicates the amount of tokens that should be considered
		virtual size_t patternLength() const = 0;
		/// \brief indicates if the sequence starting with begin matches the pattern
		virtual bool matches(std::vector<L1Token>::const_iterator begin) const = 0;
		/// \brief applies pattern to the sequence and returns the new sequence
		virtual L1Token apply(std::vector<L1Token>::const_iterator begin) const = 0;
	};

	class FloatRule final : public L1Rule
	{
	public:
		size_t patternLength() const override
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

			return (begin + 2)->onlyNumbersWithF();
		}

		L1Token apply(std::vector<L1Token>::const_iterator begin) const override
		{
			auto val = begin->getValue() + "." + (begin + 2)->getValue();
			return L1Token(L1Token::Type::Float, begin->getPosition(), val);
		}
	};

	class NumberRule : public L1Rule
	{
	public:
		size_t patternLength() const override { return 1; }
		
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
				throw std::runtime_error("unknown token");

			return *begin; // leave as is
		}
	};

	class FunctionRule : public L1Rule
	{
	public:
		size_t patternLength() const override { return 2; }
		
		bool matches(std::vector<L1Token>::const_iterator begin) const override
		{
			return begin->getType() == L1Token::Type::Identifier && (begin + 1)->getType() == L1Token::Type::BracketOpen;
		}

		L1Token apply(std::vector<L1Token>::const_iterator begin) const override
		{
			return L1Token(L1Token::Type::Function, begin->getPosition(), begin->getValue());
		}
	};
}
