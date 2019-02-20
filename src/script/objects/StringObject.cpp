#include "../../../include/script/objects/StringObject.h"
#include "../../../include/script/Util.h"

script::StringObject::StringObject(std::string value)
	:
ValueComparableObject(std::move(value))
{
	addFunction("add", std::bind(&StringObject::add, this, std::placeholders::_1));
	addFunction("getLength", Util::makeFunction(this, &StringObject::getLength, "int StringObject::getLength()"));
}

script::ScriptObject::FunctionT script::StringObject::getCtor()
{
	return [](const ArrayObjectPtr& args)
	{
		std::string res;

		// add all arguments
		for (const auto& obj : *args)
		{
			res += Util::getBareString(obj);
		}

		return std::make_shared<StringObject>(res);
	};
}

std::string script::StringObject::toString() const
{
	return "\"" + m_value + "\"";
}

script::ScriptObjectPtr script::StringObject::clone() const
{
	return std::make_shared<StringObject>(m_value);
}

script::ScriptObjectPtr script::StringObject::add(const ArrayObjectPtr& args)
{
	// add all arguments
	for (const auto& obj : *args)
	{
		m_value += Util::getBareString(obj);
	}

	return shared_from_this();
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