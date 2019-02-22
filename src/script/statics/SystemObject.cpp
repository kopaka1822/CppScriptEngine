#include "../../../include/script/statics/SystemObject.h"

script::SystemObject::SystemObject(ScriptEngine &engine)
{
	m_clock = std::make_shared<ClockObject>();
	m_engine = std::make_shared<EngineObject>(engine);

	addFunction("getClock", Util::makeFunction(this, &SystemObject::getClock, "Clock SystemObject::getClock()"));
	addFunction("getEngine", Util::makeFunction(this, &SystemObject::getEngine, "Engine SystemObject::getEngine()"));
}

std::shared_ptr<script::ClockObject> script::SystemObject::getClock() const
{
	return m_clock;
}

std::shared_ptr<script::EngineObject> script::SystemObject::getEngine() const
{
	return m_engine;
}
