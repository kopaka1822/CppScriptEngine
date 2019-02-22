#include "../../../include/script/statics/TimestampObject.h"
#include "../../../include/script/Util.h"

script::TimestampObject::TimestampObject(time::Timestamp time)
	:
ValueComparableObject(time)
{
	addFunction("add", Util::makeFunction(this, &TimestampObject::add, "TimestampObject::add(Duration)"));
	addFunction("subtract", Util::combineFunctions({
		Util::makeFunction(this, static_cast<void(TimestampObject::*)(const time::Duration&)>(&TimestampObject::subtract), "TimestampObject::subtract(Duration)"),
		Util::makeFunction(this, static_cast<time::Duration(TimestampObject::*)(const time::Timestamp&) const>(&TimestampObject::subtract), "Duration TimestampObject::subtract(Timestamp)"),
	}));
}

std::string script::TimestampObject::toString() const
{
	auto dur = m_value.time_since_epoch();
	return "Timestamp(" + std::to_string(dur.count()) + ")";
}

script::ScriptObjectPtr script::TimestampObject::clone() const
{
	return std::make_shared<TimestampObject>(m_value);
}

void script::TimestampObject::add(const time::Duration& other)
{
	m_value += other;
}

void script::TimestampObject::subtract(const time::Duration& other)
{
	m_value -= other;
}

script::time::Duration script::TimestampObject::subtract(const time::Timestamp& other) const
{
	return m_value - other;
}

template <>
script::ScriptObjectPtr script::Util::makeObject<script::time::Timestamp>(const time::Timestamp& value)
{
	return std::make_shared<TimestampObject>(value);
}
