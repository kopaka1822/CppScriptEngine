#pragma once
#include "../objects/ValueComparableObject.h"
#include "Time.h"

namespace script
{
	class DurationObject : public ValueComparableObject<time::Duration>
	{
	public:
		DurationObject(time::Duration value);
		~DurationObject() override = default;

		std::string toString() const override;
		ScriptObjectPtr clone() const override;

		float getNanoseconds() const;
		float getMicroseconds() const;
		float getMilliseconds() const;
		float getSeconds() const;
		float getMinutes() const;
	};
}
