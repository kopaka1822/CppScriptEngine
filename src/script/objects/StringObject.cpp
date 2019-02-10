#include "../../../include/script/objects/StringObject.h"
#include "../../../include/script/Util.h"

script::StringObject::StringObject(std::string value)
	:
ValueComparableObject(std::move(value))
{
	addFunction("add", [this](ScriptObjectArrayPtr args)
	{
		// add all arguments
		for (size_t i = 0; i < args->count(); ++i)
			m_value += args->get(i)->toString();

		return this->shared_from_this();
	});
	//ScriptObject::addFunction("add", Util::makeFunction(this, &StringObject::add, "StringObject::add(string)"));
}

std::string script::StringObject::toString() const
{
	return "\"" + m_value + "\"";
}

script::ScriptObjectPtr script::StringObject::clone() const
{
	return std::make_shared<StringObject>(m_value);
}

void script::StringObject::add(const std::string& value)
{
	m_value += value;
}

size_t script::StringObject::length() const
{
	return m_value.length();
}

template<>
script::ScriptObjectPtr script::Util::toScriptObject<std::string>(const std::string& value)
{
	return std::make_shared<StringObject>(value);
}