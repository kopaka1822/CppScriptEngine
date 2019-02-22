#include "../../../include/script/statics/DateTimeObject.h"
#include "../../../include/script/Util.h"

script::DateTimeObject::DateTimeObject(std::tm value)
	:
m_value(value)
{
	addFunction("getSecond", Util::makeFunction(this, &DateTimeObject::getSecond, "int DateTimeObject::getSecond()"));
	addFunction("getMinute", Util::makeFunction(this, &DateTimeObject::getMinute, "int DateTimeObject::getMinute()"));
	addFunction("getHour", Util::makeFunction(this, &DateTimeObject::getHour, "int DateTimeObject::getHour()"));
	addFunction("getDay", Util::makeFunction(this, &DateTimeObject::getDay, "int DateTimeObject::getDay()"));
	addFunction("getMonth", Util::makeFunction(this, &DateTimeObject::getMonth, "int DateTimeObject::getMonth()"));
	addFunction("getYear", Util::makeFunction(this, &DateTimeObject::getYear, "int DateTimeObject::getYear()"));
}

/*script::DateTimeObject::DateTimeObject()
{
	time_t rawtime;
	if (time(&rawtime) == -1)
		throw std::runtime_error("could not retrieve time");

	struct tm* timeinfo = localtime(&rawtime);
	memcpy(&m_value, timeinfo, sizeof(tm));
}*/

std::string script::DateTimeObject::toString() const
{
	return asctime(&m_value);
}

script::ScriptObjectPtr script::DateTimeObject::clone() const
{
	return std::make_unique<DateTimeObject>(m_value);
}

bool script::DateTimeObject::equals(const ScriptObjectPtr& other) const
{
	auto stamp = std::reinterpret_pointer_cast<DateTimeObject>(other);
	if (!stamp)
		return false;

	return memcmp(&m_value, &(stamp->m_value), sizeof(m_value)) != 0;
}

int script::DateTimeObject::getSecond() const
{
	return m_value.tm_sec;
}

int script::DateTimeObject::getMinute() const
{
	return m_value.tm_min;
}

int script::DateTimeObject::getHour() const
{
	return m_value.tm_hour;
}

int script::DateTimeObject::getDay() const
{
	return m_value.tm_mday;
}

int script::DateTimeObject::getMonth() const
{
	return m_value.tm_mon;
}

int script::DateTimeObject::getYear() const
{
	return m_value.tm_year + 1900;
}
