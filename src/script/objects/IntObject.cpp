#include "../../../include/script/objects/IntObject.h"
#include "../../../include/script/Util.h"

script::IntObject::IntObject(int value)
	:
ValueComparableObject(value)
{
	addFunction("add", Util::makeFunction(this, &IntObject::add, "IntObject::add(int)"));
	addFunction("subtract", Util::makeFunction(this, &IntObject::subtract, "IntObject::subtract(int)"));
	addFunction("divide", Util::makeFunction(this, &IntObject::divide, "IntObject::divide(int)"));
	addFunction("multiply", Util::makeFunction(this, &IntObject::multiply, "IntObject::multiply(int)"));
}

std::string script::IntObject::toString() const
{
	return std::to_string(m_value);
}

script::ScriptObjectPtr script::IntObject::clone() const
{
	return std::make_shared<IntObject>(m_value);
}

void script::IntObject::add(int i)
{
	m_value += i;
}

void script::IntObject::subtract(int i)
{
	m_value -= i;
}

void script::IntObject::divide(int i)
{
	if (i == 0)
		throw std::runtime_error("IntObject::divide division by zero");
	m_value /= i;
}

void script::IntObject::multiply(int i)
{
	m_value *= i;
}

template<>
script::ScriptPtr<script::GetValueObject<int>> script::Util::toScriptObject<int>(const int& value)
{
	return std::make_shared<IntObject>(value);
}