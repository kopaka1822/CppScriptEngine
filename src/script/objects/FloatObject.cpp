#include "../../../include/script/objects/FloatObject.h"
#include "../../../include/script/Util.h"

script::FloatObject::FloatObject(float value)
	:
NumericalObject(value)
{}

std::string script::FloatObject::toString() const
{
	return std::to_string(m_value);
}

script::ScriptObjectPtr script::FloatObject::clone() const
{
	return std::make_shared<FloatObject>(m_value);
}

template <>
script::ScriptObjectPtr script::Util::makeObject<float>(const float& value)
{
	return std::make_shared<FloatObject>(value);
}

