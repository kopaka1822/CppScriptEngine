#pragma once
#include "L1Rule.h"

namespace script
{
	/// \brief converts + (Plus) and = (assign) into += PlusAssign (same for - * /)
	class L1OperatorAssignRule final : public L1Rule
	{
	public:
		int patternLength() const override;
		bool matches(std::vector<L1Token>::const_iterator begin) const override;
		L1Token apply(std::vector<L1Token>::const_iterator begin) const override;
	};
}