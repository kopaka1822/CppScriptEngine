#pragma once
#include "../objects/ValueComparableObject.h"
#include "Time.h"

namespace script
{
	class TimestampObject : public ValueComparableObject<time::Timestamp>
	{
	public:
		explicit TimestampObject(time::Timestamp time);
		~TimestampObject() override = default;

		std::string toString() const override;
		ScriptObjectPtr clone() const override;

		void add(const time::Duration& other);
		void subtract(const time::Duration& other);
		time::Duration subtract(const time::Timestamp& other) const;
	};
}
