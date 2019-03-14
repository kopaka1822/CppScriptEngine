#include "pch.h"

#define TestSuite EnumObjectTest
using namespace script;

enum class TestEC
{
	Value1 = 0,
	Value2 = 1
};

class TestECStatic : public StaticEnumObject<TestEC>
{
	TestECStatic()
	{
		addValue(TestEC::Value1, "Value1");
		addValue(TestEC::Value2, "Value2");
	}
public:
	~TestECStatic() override = default;

	static std::shared_ptr<TestECStatic> get()
	{
		static std::shared_ptr<TestECStatic> obj(new TestECStatic);
		return obj;
	}
};

template<>
ScriptObjectPtr Util::makeObject<TestEC>(const TestEC& value)
{
	return TestECStatic::get()->parse(int(value));
}

TEST(TestSuite, StaticGetter)
{
	ScriptEngine engine;
	engine.setStaticObject("TestEC", TestECStatic::get());

	// value 1 included
	ScriptObjectPtr res;
	EXPECT_NO_THROW(res = engine.execute("TestEC.Value1"));
	EXPECT_TRUE(res);
	// should be enum object
	auto eobj = std::dynamic_pointer_cast<EnumObject<TestEC>>(res);
	EXPECT_TRUE(eobj);
	EXPECT_EQ(eobj->getValue(), TestEC::Value1);

	// value 2 included
	EXPECT_NO_THROW(res = engine.execute("TestEC.Value2"));
	EXPECT_TRUE(res);
	// should be enum object
	eobj = std::dynamic_pointer_cast<EnumObject<TestEC>>(res);
	EXPECT_TRUE(eobj);
	EXPECT_EQ(eobj->getValue(), TestEC::Value2);

	// value 3 not included
	EXPECT_THROW(engine.execute("TestEC.Value3"), std::exception);
}

TEST(TestSuite, Parse)
{
	// string to enum parsing
	ScriptPtr<EnumObject<TestEC>> res;
	EXPECT_NO_THROW(res = TestECStatic::get()->parse("Value1"));
	EXPECT_TRUE(res);
	EXPECT_EQ(res->getValue(), TestEC::Value1);

	// invalid
	EXPECT_THROW(res = TestECStatic::get()->parse("Value3"), std::exception);

	// enum to string parsing
	EXPECT_NO_THROW(res = TestECStatic::get()->parse(int(TestEC::Value2)));
	EXPECT_TRUE(res);
	EXPECT_EQ(res->getValue(), TestEC::Value2);

	// invalid
	EXPECT_THROW(res = TestECStatic::get()->parse(4), std::exception);
}

TEST(TestSuite, ToStringConsistency)
{
	ScriptEngine engine;
	engine.setStaticObject("TestEC", TestECStatic::get());

	auto v1 = engine.execute("TestEC.Value1")->toString();
	auto v2 = engine.execute("TestEC.parse(\"Value1\")")->toString();
	auto v3 = engine.execute("TestEC.parse(0)")->toString();

	EXPECT_EQ(v1, v2);
	EXPECT_EQ(v2, v3);
}

static TestEC EnumTestFunc(TestEC value)
{
	EXPECT_EQ(value, TestEC::Value1);
	return TestEC::Value2;
}

TEST(TestSuite, FunctionParameter)
{
	ScriptEngine engine;
	engine.setStaticObject("TestEC", TestECStatic::get());
	engine.setStaticFunction("EnumTestFunc", Util::makeFunction(EnumTestFunc, "enum EnumTestFunc(enum)"));

	ScriptObjectPtr res;
	ASSERT_NO_THROW(res = engine.execute("EnumTestFunc(TestEC.Value1)"));
	auto eobj = std::dynamic_pointer_cast<EnumObject<TestEC>>(res);
	EXPECT_TRUE(eobj);
	ASSERT_EQ(eobj->getValue(), TestEC::Value2);
}