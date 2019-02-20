#include "../../../include/script/objects/IntObject.h"
#include "../../../include/script/Util.h"

script::IntObject::IntObject(int value)
	:
	NumericalObject(value)
{}

script::ScriptObject::FunctionT script::IntObject::getCtor()
{
	return Util::combineFunctions({
		Util::fromLambda([](int val)
		{
			return std::make_shared<IntObject>(val);
		}, "Int(int)"),
		Util::fromLambda([](float val)
		{
			return std::make_shared<IntObject>(int(val));
		}, "Int(float)"),
		Util::fromLambda([](const std::string& val)
		{
			return std::make_shared<IntObject>(std::stoi(val));
		}, "Int(string)")
	});
}

std::string script::IntObject::toString() const
{
	return std::to_string(m_value);
}

script::ScriptObjectPtr script::IntObject::clone() const
{
	return std::make_shared<IntObject>(m_value);
}

void script::IntObject::divide(const int& other)
{
	if (other == 0)
		throw std::runtime_error("IntObject::divide division by zero");
	NumericalObject::divide(other);
}

template<>
script::ScriptObjectPtr script::Util::makeObject<int>(const int& value)
{
	return std::make_shared<IntObject>(value);
}
