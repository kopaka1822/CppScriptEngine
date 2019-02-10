#include "pch.h"

#define TestSuite IntObjectTest
using namespace script;

TEST(TestSuite, UtilityCreation) 
{
	auto obj = Util::toScriptObject(5);
	EXPECT_TRUE(obj);
	auto intObj = std::dynamic_pointer_cast<script::IntObject>(obj);
	EXPECT_TRUE(intObj);
	EXPECT_EQ(intObj->getValue(), 5);
}

TEST(TestSuite, DivisionZero)
{
	auto nom = Util::toScriptObject(5);
	auto denom = Util::toScriptObject(0);

	// division by not zero
	EXPECT_NO_THROW(nom->invoke("divide", Util::makeArray(nom)));
	// division by zero
	EXPECT_THROW(nom->invoke("divide", Util::makeArray(denom)), std::runtime_error);
}