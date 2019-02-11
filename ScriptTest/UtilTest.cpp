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

class UtilTest
{
public:
	void mutator(int a, float b)
	{
		EXPECT_EQ(a, 5);
		EXPECT_EQ(b, 6.0f);
	}
	void mutatorPtr(int* a, float b)
	{
		EXPECT_NE(a, nullptr);
		EXPECT_EQ(*a, 3);
		EXPECT_EQ(b, 2.0f);
		*a = 5;
	}
	void mutatorScript(ScriptPtr<IntObject> intObj, int a)
	{
		EXPECT_EQ(intObj->getValue(), 6);
		EXPECT_EQ(a, 9);
		intObj->add(1);
		intObj.reset();
	}
	void mutatorConstScriptRef(const ScriptPtr<IntObject>& intObj, int a)
	{
		EXPECT_EQ(intObj->getValue(), 6);
		EXPECT_EQ(a, 10);
		intObj->add(1);
	}
	void mutatorScriptRef(ScriptPtr<IntObject>& intObj, int a)
	{
		EXPECT_EQ(intObj->getValue(), 9);
		EXPECT_EQ(a, 7);
		intObj->add(1);
		intObj.reset();
	}
	void mutatorConst(int a) const
	{
		EXPECT_EQ(a, 12);
	}
	void mutatorConstPtr(const int* a)
	{
		EXPECT_EQ(*a, 13);
	}
	void mutatorRef(int& a)
	{
		EXPECT_EQ(a, 7);
		a = 9;
	}
	void mutatorConstRef(const int& a)
	{
		EXPECT_EQ(a, 14);
	}
};

class UtilTestWrapper : public ValueObject<UtilTest>
{
public:
	std::string toString() const override
	{
		return "UtilTestWrapper";
	}
	ScriptObjectPtr clone() const override
	{
		throw ObjectNotCloneableException("UtilTestDerived");
	}
};

class UtilTestDerived : GetValueObject<UtilTestDerived>
{
public:
	std::string toString() const override
	{
		return "UtilTestDerived";
	}

	ScriptObjectPtr clone() const override
	{
		throw ObjectNotCloneableException("UtilTestDerived");
	}

	UtilTestDerived& getValue() override
	{
		return *this;
	}
};

TEST(TestSuite, MakeFunctionForWrapper)
{
	auto intObj = std::dynamic_pointer_cast<IntObject>(Util::toScriptObject(1));
	
	auto w = std::make_shared<UtilTestWrapper>();
	UtilTest& t = w->getValue();
	auto func = Util::makeFunction(w.get(), &t, &UtilTest::mutator, "");
	ASSERT_TRUE(func);
	ASSERT_NO_THROW(func(Util::makeArray(5, 6.0f)));
	
	func = decltype(func)(); // assign empty function
	func = Util::makeFunction(w.get(), &t, &UtilTest::mutatorPtr, "");
	ASSERT_TRUE(func);
	intObj->getValue() = 3;
	ASSERT_NO_THROW(func(Util::makeArray(intObj, 2.0f)));
	EXPECT_EQ(intObj->getValue(), 5);

	func = decltype(func)(); // assign empty function
	func = Util::makeFunction(w.get(), &t, &UtilTest::mutatorScript, "");
	ASSERT_TRUE(func);
	intObj->getValue() = 6;
	ASSERT_NO_THROW(func(Util::makeArray(intObj, 9)));
	EXPECT_TRUE(intObj);
	EXPECT_EQ(intObj->getValue(), 7);

	func = decltype(func)(); // assign empty function
	func = Util::makeFunction(w.get(), &t, &UtilTest::mutatorConstScriptRef, "");
	ASSERT_TRUE(func);
	intObj->getValue() = 6;
	ASSERT_NO_THROW(func(Util::makeArray(intObj, 10)));
	EXPECT_EQ(intObj->getValue(), 7);

	func = decltype(func)(); // assign empty function
	func = Util::makeFunction(w.get(), &t, &UtilTest::mutatorScriptRef, "");
	ASSERT_TRUE(func);
	intObj->getValue() = 9;
	ASSERT_NO_THROW(func(Util::makeArray(intObj, 7)));
	// This should not reset the pointer!
	ASSERT_TRUE(intObj);
	EXPECT_EQ(intObj->getValue(), 10);

	//func = decltype(func)(); // assign empty function
	//func = Util::makeFunction(w.get(), &t, &UtilTest::mutatorConst, "");
	//EXPECT_TRUE(func);

	func = decltype(func)(); // assign empty function
	func = Util::makeFunction(w.get(), &t, &UtilTest::mutatorConstPtr, "");
	ASSERT_TRUE(func);
	ASSERT_NO_THROW(func(Util::makeArray(13)));

	func = decltype(func)(); // assign empty function
	func = Util::makeFunction(w.get(), &t, &UtilTest::mutatorRef, "");
	ASSERT_TRUE(func);
	intObj->getValue() = 7;
	ASSERT_NO_THROW(func(Util::makeArray(intObj)));
	EXPECT_EQ(intObj->getValue(), 9);

	func = decltype(func)(); // assign empty function
	func = Util::makeFunction(w.get(), &t, &UtilTest::mutatorConstRef, ""); 
	ASSERT_TRUE(func);
	ASSERT_NO_THROW(func(Util::makeArray(14)));
}