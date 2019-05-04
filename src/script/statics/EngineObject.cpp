#include "../../../include/script/statics/EngineObject.h"

script::EngineObject::EngineObject(ScriptEngine& engine)
	:
m_engine(engine)
{
	addFunction("execute", Util::makeFunction(this, &EngineObject::execute, "object EngineObject::execute(string)"));
	addFunction("getObject", Util::makeFunction(this, &EngineObject::getObject, "object EngineObject::getObject(string)"));
	addFunction("getStaticObject", Util::makeFunction(this, &EngineObject::getStaticObject, "object EngineObject::getStaticObject(string)"));
	addFunction("setObject", Util::makeFunction(this, &EngineObject::setObject, "EngineObject::setObject(string, object)"));
	addFunction("removeObject", Util::makeFunction(this, &EngineObject::removeObject, "EngineObject::removeObject(string)"));
	addFunction("clearObjects", Util::makeFunction(this, &EngineObject::clearObjects, "EngineObject::clearObjects()"));

	addFunction("getObjects", Util::makeFunction(this, &EngineObject::getObjects, "array EngineObject::getObjects()"));
	addFunction("getStaticObjects", Util::makeFunction(this, &EngineObject::getStaticObjects, "array EngineObject::getStaticObjects()"));
	addFunction("getStaticFunctions", Util::makeFunction(this, &EngineObject::getStaticFunctions, "array EngineObject::getStaticFunctions()"));
}

script::ScriptEngine& script::EngineObject::getValue()
{
	return m_engine;
}

script::ScriptObjectPtr script::EngineObject::execute(const std::string& command)
{
	return m_engine.execute(command);
}

script::ScriptObjectPtr script::EngineObject::getObject(const std::string& name) const
{
	return m_engine.getObject(name);
}

script::ScriptObjectPtr script::EngineObject::getStaticObject(const std::string& name) const
{
	return m_engine.getStaticObject(name);
}

void script::EngineObject::setObject(const std::string& name, ScriptObjectPtr object)
{
	m_engine.setObject(name, object);
}

void script::EngineObject::removeObject(const std::string& name)
{
	m_engine.setObject(name, nullptr);
}

void script::EngineObject::clearObjects()
{
	m_engine.clearObjects();
}

std::vector<std::string> script::EngineObject::getObjects() const
{
	const auto& objs = m_engine.getObjects();
	std::vector<std::string> res;
	res.reserve(objs.size());
	for (const auto& o : objs)
		res.push_back(o.first);

	return res;
}

std::vector<std::string> script::EngineObject::getStaticObjects() const
{
	const auto& objs = m_engine.getStaticObjects();
	std::vector<std::string> res;
	res.reserve(objs.size());
	for (const auto& o : objs)
		res.push_back(o.first);

	return res;
}

std::vector<std::string> script::EngineObject::getStaticFunctions() const
{
	const auto& objs = m_engine.getStaticFunctions();
	std::vector<std::string> res;
	res.reserve(objs.size());
	for (const auto& o : objs)
		res.push_back(o.first);

	return res;
}
