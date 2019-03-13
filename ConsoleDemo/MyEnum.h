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
	static std::shared_ptr<MyEnumStaticObject> get();
private:
	MyEnumStaticObject();
};