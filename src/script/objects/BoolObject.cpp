#include "../../../include/script/objects/BoolObject.h"
#include "../../../include/script/Util.h"

script::BoolObject::BoolObject(bool value)
	:
ValueComparableObject(value)
{
	addFunction("invert", Util::makeFunction(this, &BoolObject::invert, "BoolObject::invert()"));
}

std::string script::BoolObject::toString() const
{
	return m_value ? "true" : "false";
}

script::ScriptObjectPtr script::BoolObject::clone() const
{
	return std::make_shared<BoolObject>(m_value);
}

void script::BoolObject::invert()
{
	m_value = !m_value;
}

template<>
script::ScriptObjectPtr script::Util::makeObject<bool>(const bool& value)
{
	return std::make_shared<BoolObject>(value);
}
