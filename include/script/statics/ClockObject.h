#pragma once
#include "../objects/ScriptObject.h"
#include "DateTimeObject.h"
#include "Time.h"

namespace script
{
	class ClockObject : public ScriptObject
	{
	public:
		ClockObject();

		/// \brief returns current point in time (low resolution)
		std::tm getDate() const;
		/// \brief returns a high resolution timestamp
		time::Timestamp getTimestamp() const;
	};
}
