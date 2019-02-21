#include "pch.h"

#define TestSuite UtilTest
using namespace script;

TEST(TestSuite, ToScriptObject) 
{
	EXPECT_TRUE(Util::makeObject(1)); // integer
	EXPECT_TRUE(Util::makeObject(true)); // bool
	EXPECT_TRUE(Util::makeObject(nullptr)); // null
	EXPECT_TRUE(Util::makeObject(std::string("test"))); // string
	EXPECT_TRUE(Util::makeObject("test")); // const char*
	EXPECT_TRUE(Util::makeObject(3.14f)); // float
}

TEST(TestSuite, MakeArray)
{
	// make array with script objects
	auto intObj = Util::makeObject(1);
	auto strObj = Util::makeObject("test");

	auto arr = Util::makeArray();
	EXPECT_TRUE(arr);
	EXPECT_EQ(arr->getCount(), 0);

	EXPECT_NO_THROW(arr->add(intObj));
	//EXPECT_THROW(arr->add(nullptr), std::runtime_error);

	arr = Util::makeArray(intObj, strObj);
	EXPECT_EQ(arr->getCount(), 2);
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
		EXPECT_TRUE(a != nullptr);
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

	void mutatorConst(int* a) const
	{
		EXPECT_EQ(*a, 12);
		*a = 10;
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

	void mutatorNullptr(int* a)
	{
		EXPECT_TRUE(a == nullptr);
	}

	int returnInt(int a)
	{
		EXPECT_EQ(a, 10);
		return a + 5;
	}

	ScriptPtr<IntObject> returnIntObj(int a)
	{
		return std::reinterpret_pointer_cast<IntObject>(Util::makeObject(a));
	}

	int returnIntConst(int a) const
	{
		EXPECT_EQ(a, 10);
		return a + 5;
	}

	ScriptPtr<IntObject> returnIntObjConst(int a) const
	{
		return std::reinterpret_pointer_cast<IntObject>(Util::makeObject(a));
	}
};

class UtilTestWrapper : public ValueObject<UtilTest>
{
public:
};

// tests wrapper functions with void return value
// tests all sorts of parameters as well
TEST(TestSuite, MakeVoidFunctionForWrapper)
{
	auto intObj = std::dynamic_pointer_cast<IntObject>(Util::makeObject(1));
	
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

	func = decltype(func)(); // assign empty function
	func = Util::makeFunction(w.get(), &t, &UtilTest::mutatorConst, "");
	EXPECT_TRUE(func);
	intObj->getValue() = 12;
	ASSERT_NO_THROW(func(Util::makeArray(intObj)));
	EXPECT_EQ(intObj->getValue(), 10);

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

	func = decltype(func)(); // assign empty function
	func = Util::makeFunction(w.get(), &t, &UtilTest::mutatorNullptr, "");
	ASSERT_TRUE(func);
	ASSERT_NO_THROW(func(Util::makeArray(nullptr)));
	ASSERT_NO_THROW(func(Util::makeArray(NullObject::get())));
}

// tests wrapper functions with return value
TEST(TestSuite, MakeReturnFunctionForWrapper)
{
	auto w = std::make_shared<UtilTestWrapper>();
	UtilTest& t = w->getValue();
	auto func = Util::makeFunction(&t, &UtilTest::returnInt, "");
	ASSERT_TRUE(func);
	auto retVal = func(Util::makeArray(10));
	ASSERT_TRUE(retVal->equals(Util::makeObject(15)));

	func = Util::makeFunction(&t, &UtilTest::returnIntObj, "");
	ASSERT_TRUE(func);
	retVal = func(Util::makeArray(12));
	ASSERT_TRUE(retVal->equals(Util::makeObject(12)));

	func = Util::makeFunction(&t, &UtilTest::returnIntConst, "");
	ASSERT_TRUE(func);
	retVal = func(Util::makeArray(10));
	ASSERT_TRUE(retVal->equals(Util::makeObject(15)));

	func = Util::makeFunction(&t, &UtilTest::returnIntObjConst, "");
	ASSERT_TRUE(func);
	retVal = func(Util::makeArray(16));
	ASSERT_TRUE(retVal->equals(Util::makeObject(16)));
}

class UtilTestDerived : public ScriptObject
{
public:
	void mutator(int a, float b)
	{
		EXPECT_EQ(a, 2);
		EXPECT_EQ(b, 3.0f);
	}

	void mutatorConst(int* a, float b) const
	{
		EXPECT_TRUE(a == nullptr);
		EXPECT_EQ(b, 5.0f);
	}

	int returnInt()
	{
		return 15;
	}

	int returnIntConst() const
	{
		return 10;
	}

	ScriptPtr<IntObject> returnIntConst()
	{
		return std::reinterpret_pointer_cast<IntObject>(Util::makeObject(5));
	}

	void overload1(int a, float b)
	{
		EXPECT_EQ(a, 10);
		EXPECT_EQ(b, 1.0f);
	}
	
	void overload2(bool b)
	{
		EXPECT_EQ(b, false);
	}

	void derivedConstRef(const UtilTestDerived& ref)
	{
		ASSERT_EQ(ref.returnIntConst(), 10);
	}

	void derivedRef(UtilTestDerived& ref)
	{
		ASSERT_EQ(ref.returnInt(), 15);
	}

	void derivedPtr(UtilTestDerived* ptr)
	{
		if(ptr != nullptr)
			ASSERT_EQ(ptr->returnInt(), 15);
	}
};

// tests void functions called from a wrapper object
TEST(TestSuite, MakeVoidFunctionForDerived)
{
	auto d = std::make_shared<UtilTestDerived>();
	auto func = Util::makeFunction(d.get(), &UtilTestDerived::mutator, "");
	ASSERT_TRUE(func);
	ASSERT_NO_THROW(func(Util::makeArray(2, 3.0f)));

	func = Util::makeFunction(d.get(), &UtilTestDerived::mutatorConst, "");
	ASSERT_TRUE(func);
	ASSERT_NO_THROW(func(Util::makeArray(nullptr, 5.0f)));
}

// tests non-void functions called from a wrapper object
TEST(TestSuite, MakeReturnFunctionForDerived)
{
	auto d = std::make_shared<UtilTestDerived>();
	auto func = Util::makeFunction(d.get(), &UtilTestDerived::returnInt, "");
	ASSERT_TRUE(func);
	ASSERT_TRUE(func(Util::makeArray())->equals(Util::makeObject(15)));

	func = Util::makeFunction(d.get(), &UtilTestDerived::returnIntConst, "");
	ASSERT_TRUE(func);
	ASSERT_TRUE(func(Util::makeArray())->equals(Util::makeObject(5)));
}

TEST(TestSuite, PassDerivedAsArgument)
{
	auto d = std::make_shared<UtilTestDerived>();
	auto d2 = std::make_shared<UtilTestDerived>();
	auto func = Util::makeFunction(d.get(), &UtilTestDerived::derivedConstRef, "");
	ASSERT_TRUE(func);
	ASSERT_NO_THROW(func(Util::makeArray(d2)));

	func = Util::makeFunction(d.get(), &UtilTestDerived::derivedRef, "");
	ASSERT_TRUE(func);
	ASSERT_NO_THROW(func(Util::makeArray(d2)));

	func = Util::makeFunction(d.get(), &UtilTestDerived::derivedPtr, "");
	ASSERT_TRUE(func);
	ASSERT_NO_THROW(func(Util::makeArray(d2)));
	ASSERT_NO_THROW(func(Util::makeArray(nullptr)));
}

TEST(TestSuite, FunctionCombineTest)
{
	auto d = std::make_shared<UtilTestDerived>();
	auto o1 = Util::makeFunction(d.get(), &UtilTestDerived::overload1, "overload1");
	ASSERT_TRUE(o1);
	auto o2 = Util::makeFunction(d.get(), &UtilTestDerived::overload2, "overload2");
	ASSERT_TRUE(o2);
	auto func = Util::combineFunctions({ o1, o2 });

	// correct overload
	ASSERT_TRUE(func);
	ASSERT_NO_THROW(func(Util::makeArray(10, 1.0f)));
	ASSERT_NO_THROW(func(Util::makeArray(false)));

	// arg count
	ASSERT_THROW(func(Util::makeArray()), InvalidArgumentCount);
	ASSERT_THROW(func(Util::makeArray(10, 1.0f, 0.0f)), InvalidArgumentCount);

	// arg types
	ASSERT_THROW(func(Util::makeArray(10, 1)), InvalidArgumentType);
	ASSERT_THROW(func(Util::makeArray(nullptr)), InvalidArgumentType);
}