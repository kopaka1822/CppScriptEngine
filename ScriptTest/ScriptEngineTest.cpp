#include "pch.h"

#include "script/objects/FloatObject.h"
#include "script/Tokenizer.h"

#define TestSuite ScriptEngineTest
using namespace script;

TEST(TestSuite, VariablesTest)
{
	ScriptEngine engine;
	decltype(Tokenizer::getExecutable("")) exec;
	std::string output;

	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("a = 5"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "5");

	// primitve types
	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("b=a"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "5");

	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("b.add(8)"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "13");

	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("a"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "13");

	// floats
	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("a = 12.0f"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());

	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("a = 12f"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());

	// string
	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("myVar =\"test\""));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "\"test\"");

	// bool
	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("6.equals(6)"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "true");

	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("6.0f.equals(6)"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "false");

	// null
	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("nll = null"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "null");

	ASSERT_THROW(exec = Tokenizer::getExecutable("null = null"), ParseError);

	// brackets
	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("([([3, 5, 8].get((1))), 9].get(0))"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "5");
}

TEST(TestSuite, NumbersTest)
{
	ScriptEngine engine;
	decltype(Tokenizer::getExecutable("")) exec;
	std::string output;

	// basic math rules
	ASSERT_NO_THROW(exec = Tokenizer::getExecutable(" 5 + 10"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "15");

	ASSERT_NO_THROW(exec = Tokenizer::getExecutable(" 5 + 2 * 2"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "9");

	ASSERT_NO_THROW(exec = Tokenizer::getExecutable(" 5 + 2 / 2"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "6");

	ASSERT_NO_THROW(exec = Tokenizer::getExecutable(" 5 - 2*2"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "1");

	ASSERT_NO_THROW(exec = Tokenizer::getExecutable(" -5 + 10"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "5");

	ASSERT_NO_THROW(exec = Tokenizer::getExecutable(" 5 + -10"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "-5");

	// basic operator cloning
	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("a = 5 + (b = 10)"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "15");


	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("a + b"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "25");

	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("a"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "15");
}

TEST(TestSuite, NumbersAssingTest)
{
	ScriptEngine engine;
	decltype(Tokenizer::getExecutable("")) exec;
	std::string output;

	// basic math rules
	ASSERT_NO_THROW(exec = Tokenizer::getExecutable(" 5 += 10"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "15");

	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("a = 6"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "6");

	ASSERT_NO_THROW(exec = Tokenizer::getExecutable(" a /= 1 * 2"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "3");

	ASSERT_NO_THROW(exec = Tokenizer::getExecutable(" 1 + a *= b = 2"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "7");

	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("b"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "2");

	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("a"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "6");

	// basic operator cloning
	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("a -= -1"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "7");
}

TEST(TestSuite, PropertiesTest)
{
	ScriptEngine engine;
	decltype(Tokenizer::getExecutable("")) exec;
	std::string output;

	// getter
	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("[1, 2, 5].Count"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "3");

	// TODO add setter

	// static property
	engine.setStaticObject("Dummy", Util::makeObject("StaticProp"));
	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("b = Dummy"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "\"StaticProp\"");

	ASSERT_NO_THROW(exec = Tokenizer::getExecutable("Dummy.Length"));
	ASSERT_NO_THROW(output = exec->execute(engine)->toString());
	ASSERT_EQ(output, "10");
}

TEST(TestSuite, RemoveObjectsTest)
{
	ScriptEngine engine;
	auto obj1 = script::Util::makeObject(20);
	auto obj2 = script::Util::makeObject(20);
	auto obj3 = script::Util::makeObject(30);
	engine.setObject("obj1", obj1);
	engine.setObject("obj2", obj2);
	engine.setObject("obj3", obj3);
	engine.setObject("obj4", obj3);

	EXPECT_EQ(engine.getObjects().size(), 4);

	engine.setObject("obj1", nullptr);
	EXPECT_EQ(engine.getObjects().size(), 3);

	engine.removeObjectVariables(obj3);
	EXPECT_EQ(engine.getObjects().size(), 1);
}

TEST(TestSuite, GetObjectsTest)
{
	ScriptEngine engine;
	auto obj1 = script::Util::makeObject(20);
	auto obj2 = script::Util::makeObject(30.0f);
	engine.setObject("obj1", obj1);
	engine.setObject("obj2", obj2);

	EXPECT_EQ(engine.getObjects().size(), 2);

	// equal test
	EXPECT_EQ(engine.getObject("obj1"), obj1);
	EXPECT_EQ(engine.getObject("obj2"), obj2);

	// null test
	EXPECT_EQ(engine.getObject("obj3"), nullptr);

	// same type
	EXPECT_NO_THROW(engine.getObject<IntObject>("obj1"));
	EXPECT_NO_THROW(engine.getObject<FloatObject>("obj2"));

	// invalid type
	EXPECT_THROW(engine.getObject<FloatObject>("obj1"), Exception);

	// not found
	EXPECT_EQ(engine.getObject<FloatObject>("obj3"), nullptr);

	// null object
	engine.setObject("obj1", script::NullObject::get());
	EXPECT_EQ(engine.getObject<FloatObject>("obj1"), nullptr);
}