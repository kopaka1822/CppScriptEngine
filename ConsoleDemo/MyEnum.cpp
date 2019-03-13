#include "MyEnum.h"

MyEnumStaticObject::MyEnumStaticObject()
{
	addValue(MyEnum::Value1, "Value1");
	addValue(MyEnum::Value2, "Value2");
	addValue(MyEnum::AbCd, "AbCd");
	addValue(MyEnum::EfGh, "EfGh");
}

std::shared_ptr<MyEnumStaticObject> MyEnumStaticObject::get()
{
	static std::shared_ptr<MyEnumStaticObject> obj(new MyEnumStaticObject);
	return obj;
}

template<>
script::ScriptObjectPtr script::Util::makeObject<MyEnum>(const MyEnum& value)
{
	return MyEnumStaticObject::get()->parse(int(value));
}