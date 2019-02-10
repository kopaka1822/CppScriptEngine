#include "pch.h"

#define TestSuite ScriptObjectArrayTest
using namespace script;

TEST(TestSuite, ArrayBounds)
{
	auto intObj = Util::toScriptObject(1);
	auto strObj = Util::toScriptObject("test");
	
	auto arr = Util::makeArray(intObj, strObj);
	EXPECT_EQ(arr->count(), 2);

	EXPECT_NO_THROW(arr->get(0));
	EXPECT_NO_THROW(arr->get(1));
	EXPECT_THROW(arr->get(-1), std::out_of_range);
	EXPECT_THROW(arr->get(2), std::out_of_range);
}

TEST(TestSuite, DeepCopy)
{
	auto obj = Util::toScriptObject(1);
	auto intObj = std::dynamic_pointer_cast<IntObject>(obj);
	EXPECT_TRUE(intObj);

	auto arr = Util::makeArray(intObj);
	// add 1 to the integer
	arr->get(0)->invoke("add", arr);
	// array should contain an int with the value of 2
	EXPECT_EQ(intObj->getValue(), 2);

	// deep copy
	auto copy = arr->clone();
	// add 2 to the first element of the cloned array
	copy->invoke("get", Util::makeArray(0))->invoke("add", arr);
	// now the element should differ from the other array
	
	EXPECT_EQ(intObj->getValue(), 2);
	EXPECT_TRUE(arr->get(0)->equals(intObj));
	EXPECT_FALSE(copy->invoke("get", Util::makeArray(0))->equals(intObj));
}