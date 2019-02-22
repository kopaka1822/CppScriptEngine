#include "../../../include/script/statics/SystemObject.h"

script::SystemObject::SystemObject()
{
	m_clock = std::make_shared<ClockObject>();

	addFunction("getClock", Util::makeFunction(this, &SystemObject::getClock, "Clock SystemObject::getClock()"));
}

std::shared_ptr<script::ClockObject> script::SystemObject::getClock() const
{
	return m_clock;
}
