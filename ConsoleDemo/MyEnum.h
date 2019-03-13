#pragma once
#include <script/ScriptEngine.h>

enum class MyEnum
{
	Value1,
	Value2,
	AbCd,
	EfGh
};

class MyEnumStaticObject : public script::StaticEnumObject<MyEnum>
{
public:
	MyEnumStaticObject()
	{
		addValue(MyEnum::Value1, "Value1");
		addValue(MyEnum::Value2, "Value2");
		addValue(MyEnum::AbCd, "AbCd");
		addValue(MyEnum::EfGh, "EfGh");
	}
};