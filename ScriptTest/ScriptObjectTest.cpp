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

	auto funcs = dummy->getFunctions();
	ASSERT_GE(int(funcs.size()), 1);
	ASSERT_TRUE(std::any_of(funcs.begin(), funcs.end(), [](auto name) {return name == "test"; }));
}

class Dummy : public ScriptObject
{
public:
	Dummy()
	{
		addGetterSetter("Var", m_variable);
	}

private:
	int m_variable = 20;
};

TEST(TestSuite, GetterSetter)
{
	

	auto dummy = std::make_shared<Dummy>();
	ASSERT_TRUE(dummy);

	// test availability
	auto funcs = dummy->getFunctions();
	ASSERT_TRUE(std::find(funcs.begin(), funcs.end(), "getVar") != funcs.end());
	ASSERT_TRUE(std::find(funcs.begin(), funcs.end(), "setVar") != funcs.end());

	// test set get
	dummy->invoke("setVar", Util::makeArray(10));
	auto res = dummy->invoke("getVar", Util::makeArray());
	EXPECT_TRUE(res);
	auto iobj = dynamic_cast<IntObject*>(res.get());
	EXPECT_TRUE(iobj);
	EXPECT_EQ(iobj->getValue(), 10);

	// test function signature
	try
	{
		dummy->invoke("setVar", Util::makeArray());
		EXPECT_TRUE(false);
	}
	catch (const std::exception& e)
	{
		std::string msg = e.what();
		std::string funcSig = "Dummy::setVar(int)";
		EXPECT_TRUE(msg.find(funcSig) != std::string::npos);
	}

	try
	{
		dummy->invoke("getVar", Util::makeArray(1));
		EXPECT_TRUE(false);
	}
	catch(const std::exception& e)
	{
		std::string msg = e.what();
		std::string funcSig = "int Dummy::getVar()";
		EXPECT_TRUE(msg.find(funcSig) != std::string::npos);
	}
}