#include "../../../include/script/objects/ScriptObject.h"
#include "../../../include/script/objects/ScriptObjectArray.h"
#include "../../../include/script/objects/StringObject.h"
#include "../../../include/script/objects/BoolObject.h"
#include "../../../include/script/Util.h"

script::ScriptObject::ScriptObject()
{
	addFunction("clone", [this](ScriptObjectArrayPtr args)
	{
		if (args->count() != 0)
			throw std::runtime_error("invalid argument count ScriptObject::clone()");

		return this->clone();
	});
	addFunction("toString", [this](ScriptObjectArrayPtr args)
	{
		if (args->count() != 0)
			throw std::runtime_error("invalid argument count ScriptObject::toString()");

		return std::make_shared<StringObject>(toString());
	});
	//addFunction("equals", Util::makeReturnFunction(this, &ScriptObject::equals, "ScriptObject::equals(object)"));
}

bool script::ScriptObject::equals(const ScriptObjectPtr& other) const
{
	// reference equals
	return this == other.get();
}

std::string script::ScriptObject::type() const
{
	return typeid(*this).name();
}

size_t script::ScriptObject::hashCode() const
{
	return typeid(*this).hash_code();
}

script::ScriptObjectPtr script::ScriptObject::invoke(const std::string& funcName,
	const ScriptPtr<ScriptObjectArray>& args)
{
	const auto it = m_functions.find(funcName);
	if (it == m_functions.end())
		throw std::runtime_error("ScriptObject::invoke function " + funcName + " not found");
	
	return it->second(args);
}

void script::ScriptObject::addFunction(const std::string& name, const FunctionT& func)
{
	// elements already inside?
	if (m_functions.find(name) != m_functions.end())
		throw std::runtime_error("ScriptObject::addFunction name already used: " + name);

	m_functions[name] = func;
}