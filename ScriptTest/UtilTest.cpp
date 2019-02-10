#include "pch.h"

#define TestSuite UtilTest
using namespace script;

TEST(TestSuite, ToScriptObject) 
{
	EXPECT_TRUE(Util::toScriptObject(1)); // integer
	EXPECT_TRUE(Util::toScriptObject(true)); // bool
	EXPECT_TRUE(Util::toScriptObject(nullptr)); // null
	EXPECT_TRUE(Util::toScriptObject(std::string("test"))); // string
	EXPECT_TRUE(Util::toScriptObject("test")); // const char*
	EXPECT_TRUE(Util::toScriptObject(3.14f)); // float
}

TEST(TestSuite, MakeArray)
{
	// make array with script objects
	auto intObj = Util::toScriptObject(1);
	auto strObj = Util::toScriptObject("test");

	auto arr = Util::makeArray();
	EXPECT_TRUE(arr);
	EXPECT_EQ(arr->count(), 0);

	EXPECT_NO_THROW(arr->add(intObj));
	EXPECT_THROW(arr->add(nullptr), std::runtime_error);

	arr = Util::makeArray(intObj, strObj);
	EXPECT_EQ(arr->count(), 2);
	EXPECT_TRUE(arr->get(0)->equals(intObj));
	EXPECT_TRUE(arr->get(1)->equals(strObj));

	// make array with mixed objects
	arr = Util::makeArray(2, "test2");
	EXPECT_EQ(arr->get(0)->type(), intObj->type());
	EXPECT_EQ(arr->get(1)->type(), strObj->type());

	arr = Util::makeArray("test3", intObj);
	arr = Util::makeArray(nullptr);
	EXPECT_TRUE(arr->get(0)->equals(NullObject::get()));
}

TEST(TestSuite, MakeFunction)
{
	
}