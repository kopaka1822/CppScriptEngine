#include "../../../include/script/objects/BoolObject.h"
#include "../../../include/script/Util.h"

script::BoolObject::BoolObject(bool value)
	:
ValueComparableObject(value)
{
	addFunction("invert", Util::makeFunction(this, &BoolObject::invert, "BoolObject::invert()"));
}

script::ScriptObject::FunctionT script::BoolObject::getCtor()
{
	return Util::combineFunctions({
		Util::fromLambda([](bool val)
		{
			return std::make_shared<BoolObject>(val);
		}, "Bool(bool)"),
		Util::fromLambda([](int val)
		{
			return std::make_shared<BoolObject>(val != 0);
		}, "Bool(int)"),
		Util::fromLambda([](const std::string& val)
		{
			if (val == "true") return std::make_shared<BoolObject>(true);
			if (val == "false") return std::make_shared<BoolObject>(false);
			throw std::runtime_error(val + " is neither \"true\" nor \"false\"");
		}, "Bool(string)")
	});
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
