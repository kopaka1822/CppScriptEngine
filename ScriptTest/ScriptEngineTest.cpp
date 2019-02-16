#include "pch.h"
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