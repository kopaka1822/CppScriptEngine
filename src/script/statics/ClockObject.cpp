#include "../../../include/script/statics/ClockObject.h"
#include "../../../include/script/Util.h"

script::ClockObject::ClockObject()
{
	addFunction("now", Util::makeFunction(this, &ClockObject::now, "DateTime ClockObject::now()"));
}

std::shared_ptr<script::DateTimeObject> script::ClockObject::now() const
{
	time_t rawtime;
	if (time(&rawtime) == -1)
		throw std::runtime_error("could not retrieve time");

	struct tm* timeinfo = localtime(&rawtime);

	return std::make_shared<DateTimeObject>(*timeinfo);
}
