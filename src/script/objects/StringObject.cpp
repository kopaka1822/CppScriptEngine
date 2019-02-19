#include "../../../include/script/objects/StringObject.h"
#include "../../../include/script/Util.h"

script::StringObject::StringObject(std::string value)
	:
ValueComparableObject(std::move(value))
{
	addFunction("add", [this](const ArrayObjectPtr& args)
	{
		// add all arguments
		for (const auto& obj : *args)
		{
			m_value += Util::getBareString(obj);
		}

		return this->shared_from_this();
	});
	addFunction("getLength", Util::makeFunction(this, &StringObject::getLength, "int getLength"));
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

int script::StringObject::getLength() const
{
	return int(m_value.length());
}

template<>
script::ScriptObjectPtr script::Util::makeObject<std::string>(const std::string& value)
{
	return std::make_shared<StringObject>(value);
}

script::ScriptObjectPtr script::Util::makeObject(const char* text)
{
	return std::make_shared<StringObject>(std::string(text));
}