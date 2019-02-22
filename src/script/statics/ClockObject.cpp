#include "../../../include/script/statics/ClockObject.h"
#include "../../../include/script/Util.h"
#include <ctime>

script::ClockObject::ClockObject()
{
	addFunction("getDate", Util::makeFunction(this, &ClockObject::getDate, "DateTime ClockObject::getDate()"));
	addFunction("getTimestamp", Util::makeFunction(this, &ClockObject::getTimestamp, "Timestamp ClockObject::getTimestamp()"));
}

std::shared_ptr<script::DateTimeObject> script::ClockObject::getDate() const
{
	time_t rawtime;
	if (::time(&rawtime) == -1)
		throw std::runtime_error("could not retrieve time");

	struct tm* timeinfo = localtime(&rawtime);

	return std::make_shared<DateTimeObject>(*timeinfo);
}

script::time::Timestamp script::ClockObject::getTimestamp() const
{
	return std::chrono::high_resolution_clock::now();
}
