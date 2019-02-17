#pragma once
#include "L1Rule.h"

namespace script
{
	/// \brief for floats with a dot: 3.4f 1.f 1224.2434f
	class L1FloatRule1 final : public L1Rule
	{
	public:
		int patternLength() const override;
		bool matches(std::vector<L1Token>::const_iterator begin) const override;
		L1Token apply(std::vector<L1Token>::const_iterator begin) const override;
	};
}