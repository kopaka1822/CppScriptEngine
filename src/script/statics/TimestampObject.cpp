#include "../../../include/script/statics/DateTimeObject.h"
#include <ctime>

script::DateTimeObject::DateTimeObject()
{
	time_t rawtime;
	if (time(&rawtime) == -1)
		throw std::runtime_error("could not retrieve time");

	struct tm* timeinfo = localtime(&rawtime);
	memcpy(&m_value, timeinfo, sizeof(tm));
}

std::string script::DateTimeObject::toString() const
{
	return "";
}

bool script::DateTimeObject::equals(const ScriptObjectPtr& other) const
{
	auto stamp = std::reinterpret_pointer_cast<DateTimeObject>(other);
	if (!stamp)
		return false;

	return memcmp(&m_value, &(stamp->m_value), sizeof(m_value)) != 0;
}

int script::DateTimeObject::getSeconds() const
{
	return m_value.tm_sec;
}

int script::DateTimeObject::getMinutes() const
{
	return m_value.tm_min;
}

int script::DateTimeObject::getHours() const
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
