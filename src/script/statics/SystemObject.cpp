#include "../../../include/script/statics/SystemObject.h"

script::SystemObject::SystemObject(ScriptEngine &engine)
{
	m_clock = std::make_shared<ClockObject>();
	m_engine = std::make_shared<EngineObject>(engine);
	m_io = std::make_shared<IOObject>(engine);

	addFunction("getClock", Util::makeFunction(this, &SystemObject::getClock, "Clock SystemObject::getClock()"));
	addFunction("getEngine", Util::makeFunction(this, &SystemObject::getEngine, "Engine SystemObject::getEngine()"));
	addFunction("getIO", Util::makeFunction(this, &SystemObject::getIO, "IO SystemObject::getIO()"));
	addFunction("silent", Util::makeFunction(this, &SystemObject::silent, "null SystemObject::silent()"));
}

std::shared_ptr<script::ClockObject> script::SystemObject::getClock() const
{
	return m_clock;
}

std::shared_ptr<script::EngineObject> script::SystemObject::getEngine() const
{
	return m_engine;
}

std::shared_ptr<script::IOObject> script::SystemObject::getIO() const
{
	return m_io;
}

script::ScriptObjectPtr script::SystemObject::silent(const ScriptObjectPtr&) const
{
	return NullObject::get();
}
