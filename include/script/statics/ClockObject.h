#pragma once
#include "../objects/ScriptObject.h"
#include "DateTimeObject.h"

namespace script
{
	class ClockObject : public ScriptObject
	{
	public:
		ClockObject();

		/// \brief returns current point in time
		ScriptPtr<DateTimeObject> now() const;
	};
}
