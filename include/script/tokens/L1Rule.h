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
		int patternLength() const override;
		bool matches(std::vector<L1Token>::const_iterator begin) const override;
		L1Token apply(std::vector<L1Token>::const_iterator begin) const override;
	};

	/// \brief rule for .5f .25f etc. (floats starting with a point)
	class L1FloatRule2 final : public L1Rule
	{
	public:
		int patternLength() const override;
		bool matches(std::vector<L1Token>::const_iterator begin) const override;
		L1Token apply(std::vector<L1Token>::const_iterator begin) const override;
	};

	/// \brief for int, float, bool, null or identifier (without a dot)
	class L1NumberRule final : public L1Rule
	{
	public:
		int patternLength() const override;
		bool matches(std::vector<L1Token>::const_iterator begin) const override;
		L1Token apply(std::vector<L1Token>::const_iterator begin) const override;
	};

	class L1FunctionRule final : public L1Rule
	{
	public:
		int patternLength() const override;
		bool matches(std::vector<L1Token>::const_iterator begin) const override;
		L1Token apply(std::vector<L1Token>::const_iterator begin) const override;
	};

	/// \brief converts + (Plus) and = (assign) into += PlusAssign (same for - * /)
	class L1OperatorAssignRule final : public L1Rule
	{
	public:
		int patternLength() const override;
		bool matches(std::vector<L1Token>::const_iterator begin) const override;
		L1Token apply(std::vector<L1Token>::const_iterator begin) const override;
	};

	class L1IdentifierAssignRule final : public L1Rule
	{
	public:
		int patternLength() const override;
		bool matches(std::vector<L1Token>::const_iterator begin) const override;
		L1Token apply(std::vector<L1Token>::const_iterator begin) const override;
	};
}
