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
}
