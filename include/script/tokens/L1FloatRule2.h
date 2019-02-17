#pragma once
#include "L1Rule.h"

namespace script
{
	/// \brief rule for .5f .25f etc. (floats starting with a point)
	class L1FloatRule2 final : public L1Rule
	{
	public:
		int patternLength() const override;
		bool matches(std::vector<L1Token>::const_iterator begin) const override;
		L1Token apply(std::vector<L1Token>::const_iterator begin) const override;
	};
}