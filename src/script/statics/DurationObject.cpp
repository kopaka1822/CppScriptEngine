#include "../../../include/script/statics/DurationObject.h"
#include "../../../include/script/Util.h"

script::DurationObject::DurationObject(time::Duration value)
	:
ValueComparableObject(value)
{
	addFunction("getNanoseconds", Util::makeFunction(this, &DurationObject::getNanoseconds, "float DurationObject::getNanoseconds()"));
	addFunction("getMicroseconds", Util::makeFunction(this, &DurationObject::getMicroseconds, "float DurationObject::getMicroseconds()"));
	addFunction("getMilliseconds", Util::makeFunction(this, &DurationObject::getMilliseconds, "float DurationObject::getMilliseconds()"));
	addFunction("getSeconds", Util::makeFunction(this, &DurationObject::getSeconds, "float DurationObject::getSeconds()"));
	addFunction("getMinutes", Util::makeFunction(this, &DurationObject::getMinutes, "float DurationObject::getMinutes()"));
}

std::string script::DurationObject::toString() const
{
	return std::to_string(getMilliseconds()) + " ms";
}

script::ScriptObjectPtr script::DurationObject::clone() const
{
	return std::make_shared<DurationObject>(m_value);
}

float script::DurationObject::getNanoseconds() const
{
	return std::chrono::duration<float, std::chrono::nanoseconds::period>(m_value).count();
}

float script::DurationObject::getMicroseconds() const
{
	return std::chrono::duration<float, std::chrono::microseconds::period>(m_value).count();
}

float script::DurationObject::getMilliseconds() const
{
	return std::chrono::duration<float, std::chrono::milliseconds::period>(m_value).count();
}

float script::DurationObject::getSeconds() const
{
	return std::chrono::duration<float, std::chrono::seconds::period>(m_value).count();
}

float script::DurationObject::getMinutes() const
{
	return std::chrono::duration<float, std::chrono::minutes::period>(m_value).count();
}

template <>
script::ScriptObjectPtr script::Util::makeObject<script::time::Duration>(const time::Duration& value)
{
	return std::make_shared<DurationObject>(value);
}
