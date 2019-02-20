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

std::shared_ptr<script::FloatObject> fromFloat(float val)
{
	return std::make_shared<script::FloatObject>(val);
}

script::ScriptObject::FunctionT script::FloatObject::getCtor()
{
	return Util::combineFunctions({
		Util::fromLambda([](float val)
		{
			return std::make_shared<FloatObject>(val);
		}, "Float(float)"),
		Util::fromLambda([](int val)
		{
			return std::make_shared<FloatObject>(float(val));
		}, "Float(int)"),
		Util::fromLambda([](const std::string& val)
		{
			return std::make_shared<FloatObject>(std::stof(val));
		}, "Float(string)"),
	});
}

template <>
script::ScriptObjectPtr script::Util::makeObject<float>(const float& value)
{
	return std::make_shared<FloatObject>(value);
}

