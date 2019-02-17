#include "pch.h"

#define TestSuite ScriptObjectTest
using namespace script;

TEST(TestSuite, Clone)
{
	auto intObj = Util::makeObject(23);
	auto clone = intObj->invoke("clone", Util::makeArray());
	ASSERT_TRUE(clone.get() != intObj.get());
}

TEST(TestSuite, toString)
{
	auto intObj = Util::makeObject(23);

	auto str = intObj->invoke("toString", Util::makeArray());
	ASSERT_TRUE(str->equals(Util::makeObject(intObj->toString())));
}

TEST(TestSuite, Equals)
{
	auto intObj = Util::makeObject(23);

	auto b = intObj->invoke("equals", Util::makeArray(23));
	ASSERT_TRUE(b->equals(Util::makeObject(true)));

	b = intObj->invoke("equals", Util::makeArray(2));
	ASSERT_TRUE(b->equals(Util::makeObject(false)));
}

TEST(TestSuite, AddFunction)
{
	class Dummy : public ScriptObject
	{
	public:
		Dummy()
		{}

		void init()
		{
			ASSERT_NO_THROW(addFunction("test", FunctionT()));
			// empty
			ASSERT_THROW(addFunction("", FunctionT()), std::runtime_error);
			// uppercase
			ASSERT_THROW(addFunction("Test", FunctionT()), std::runtime_error);
			// whitespaces
			ASSERT_THROW(addFunction(" test", FunctionT()), std::runtime_error);
			ASSERT_THROW(addFunction(" test ", FunctionT()), std::runtime_error);
			ASSERT_THROW(addFunction(" t st", FunctionT()), std::runtime_error);
			// already used
			ASSERT_THROW(addFunction("test", FunctionT()), std::runtime_error);
		}
	};

	auto dummy = std::make_shared<Dummy>();
	ASSERT_TRUE(dummy);
	
	dummy->init();

	auto funcs = dummy->getFunctionNames();
	ASSERT_GE(int(funcs.size()), 1);
	ASSERT_TRUE(std::any_of(funcs.begin(), funcs.end(), [](auto name) {return name == "test"; }));
}