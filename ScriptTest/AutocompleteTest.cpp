#include "pch.h"

#define TestSuite AutocompleteTest
using namespace script;

// helper functions that checks if a string is within the vector of strings
bool contains(const std::vector<std::string>& vec, const std::string& value)
{
	return std::find(vec.begin(), vec.end(), value) != vec.end();
}

void addSuffix(std::vector<std::string>& vec, const std::string& suffix)
{
	for (auto& s : vec)
		s = suffix + s;
}

TEST(TestSuite, Variables)
{
	ScriptEngine engine(ScriptEngine::PrimitiveConstructor);
	engine.setObject("obj1", Util::makeObject(1));
	engine.setObject("foo", Util::makeObject(1));
	engine.setObject("object", Util::makeObject(1));

	// single letter
	auto res = engine.getAutocomplete("o");
	addSuffix(res, "o");
	EXPECT_TRUE(contains(res, "obj1"));
	EXPECT_TRUE(contains(res, "object"));
	EXPECT_FALSE(contains(res, "foo"));

	// multiple letters
	res = engine.getAutocomplete("obje");
	addSuffix(res, "obje");
	EXPECT_FALSE(contains(res, "obj1"));
	EXPECT_TRUE(contains(res, "object"));
	EXPECT_FALSE(contains(res, "foo"));

	// entire name
	res = engine.getAutocomplete("object");
	EXPECT_FALSE(contains(res, "obj1"));
	EXPECT_FALSE(contains(res, "object"));
	EXPECT_FALSE(contains(res, "foo"));

	// nothing
	res = engine.getAutocomplete("");
	EXPECT_TRUE(contains(res, "obj1"));
	EXPECT_TRUE(contains(res, "object"));
	EXPECT_TRUE(contains(res, "foo"));
}

TEST(TestSuite, StaticVariables)
{
	ScriptEngine engine(ScriptEngine::All);
	engine.setObject("obj1", Util::makeObject(1));
	engine.setObject("coo", Util::makeObject(1));
	engine.setObject("object", Util::makeObject(1));

	// nothing
	auto res = engine.getAutocomplete("");
	EXPECT_TRUE(contains(res, "obj1"));
	EXPECT_TRUE(contains(res, "object"));
	EXPECT_TRUE(contains(res, "coo"));
	EXPECT_TRUE(contains(res, "System"));
	EXPECT_TRUE(contains(res, "Console"));

	// something
	res = engine.getAutocomplete("C");
	EXPECT_FALSE(contains(res, "oo"));
	EXPECT_FALSE(contains(res, "coo"));
	EXPECT_FALSE(contains(res, "ystem"));
	EXPECT_FALSE(contains(res, "System"));
	EXPECT_TRUE(contains(res, "onsole"));

	res = engine.getAutocomplete("Console");
	EXPECT_FALSE(contains(res, "Console"));
}

TEST(TestSuite, StaticFunctions)
{
	ScriptEngine engine(ScriptEngine::PrimitiveConstructor);

	// nothing
	auto res = engine.getAutocomplete("");
	EXPECT_TRUE(contains(res, "Int("));
	EXPECT_TRUE(contains(res, "Float("));
	EXPECT_TRUE(contains(res, "Bool("));

	res = engine.getAutocomplete("I");
	EXPECT_TRUE(contains(res, "nt("));
	EXPECT_FALSE(contains(res, "Int("));
	EXPECT_FALSE(contains(res, "Bool("));
	EXPECT_FALSE(contains(res, "ool("));
}

TEST(TestSuite, PrimitiveFunctions)
{
	ScriptEngine engine(ScriptEngine::PrimitiveConstructor);

	// nothing
	auto res = engine.getAutocomplete("5.");
	EXPECT_TRUE(contains(res, "add(")); // add
	EXPECT_TRUE(contains(res, "Functions")); // add

	// nothing
	res = engine.getAutocomplete("\"string\".");
	EXPECT_TRUE(contains(res, "getLength("));
	EXPECT_TRUE(contains(res, "Length"));

	// something
	res = engine.getAutocomplete("5.a");
	EXPECT_TRUE(contains(res, "dd(")); // add
	EXPECT_FALSE(contains(res, "Functions"));
}

TEST(TestSuite, ObjectFunctions)
{
	ScriptEngine engine(ScriptEngine::PrimitiveConstructor);
	engine.setObject("myVar", Util::makeObject("test"));

	// nothing
	auto res = engine.getAutocomplete("myVar.");
	EXPECT_TRUE(contains(res, "add("));
	EXPECT_TRUE(contains(res, "Functions"));

	res = engine.getAutocomplete("myVar.a");
	EXPECT_TRUE(contains(res, "dd("));
	EXPECT_FALSE(contains(res, "Functions"));

}

TEST(TestSuite, StaticObjectFunctions)
{
	ScriptEngine engine(ScriptEngine::All);

	auto res = engine.getAutocomplete("System.");
	EXPECT_TRUE(contains(res, "Functions"));
	EXPECT_TRUE(contains(res, "silent("));
}

TEST(TestSuite, ComplexInput)
{
	ScriptEngine engine(ScriptEngine::All);

	auto res = engine.getAutocomplete("(a + 3).add(45, System.");
	EXPECT_TRUE(contains(res, "Functions"));
	EXPECT_TRUE(contains(res, "silent("));

	// string
	res = engine.getAutocomplete("a = \"ob");
	EXPECT_TRUE(res.empty());
}