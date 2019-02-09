#include "../../../include/script/objects/BoolObject.h"
#include "../../../include/script/Util.h"

script::BoolObject::BoolObject(bool value)
	:
ValueObject(value)
{

}

std::string script::BoolObject::toString() const
{
	return m_value ? "true" : "false";
}

script::ScriptObjectPtr script::BoolObject::clone() const
{
	return std::make_shared<BoolObject>(m_value);
}

template<>
script::ScriptPtr<script::GetValueObject<bool>> script::Util::toScriptObject<bool>(const bool& value)
{
	return std::make_shared<BoolObject>(value);
}