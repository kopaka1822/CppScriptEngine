#include "pch.h"

#define TestSuite ScriptObjectArrayTest
using namespace script;

TEST(TestSuite, ArrayBounds)
{
	auto intObj = Util::makeObject(1);
	auto strObj = Util::makeObject("test");
	
	auto arr = Util::makeArray(intObj, strObj);
	EXPECT_EQ(arr->count(), 2);

	EXPECT_NO_THROW(arr->get(0));
	EXPECT_NO_THROW(arr->get(1));
	EXPECT_THROW(arr->get(-1), std::out_of_range);
	EXPECT_THROW(arr->get(2), std::out_of_range);
}

TEST(TestSuite, DeepCopy)
{
	auto obj = Util::makeObject(1);
	auto intObj = std::dynamic_pointer_cast<IntObject>(obj);
	EXPECT_TRUE(intObj);

	auto arr = Util::makeArray(intObj);
	// add 1 to the integer
	arr->get(0)->invoke("add", arr);
	// array should contain an int with the value of 2
	EXPECT_EQ(intObj->getValue(), 2);

	// deep copy
	auto copy = arr->clone();
	// add 2 to the first element of the cloned array
	copy->invoke("get", Util::makeArray(0))->invoke("add", arr);
	// now the element should differ from the other array
	
	EXPECT_EQ(intObj->getValue(), 2);
	EXPECT_TRUE(arr->get(0)->equals(intObj));
	EXPECT_FALSE(copy->invoke("get", Util::makeArray(0))->equals(intObj));

	// special case (array contains itself)
	/*arr = Util::makeArray(1, 2);
	arr->add(arr);

	ASSERT_THROW(arr->clone(), std::runtime_error);*/
}

TEST(TestSuite, Slice)
{
	auto arr = Util::makeArray(3, 5, 7, 9, 11);
	auto slice = arr->slice(1, 2);
	// count of 2
	ASSERT_EQ(slice->count(), 2);
	// elements are shallow copy
	ASSERT_TRUE(slice->get(0).get() == arr->get(1).get());
	ASSERT_TRUE(slice->get(1).get() == arr->get(2).get());

	// bounds check from
	ASSERT_THROW(arr->slice(-1, 2), std::out_of_range);
	ASSERT_THROW(arr->slice(100, 2), std::out_of_range);

	ASSERT_NO_THROW(arr->slice(1, 0));
	// count negative
	ASSERT_THROW(arr->slice(1, -1), std::runtime_error);
	// count to big
	ASSERT_THROW(arr->slice(1, 5), std::out_of_range);
}

TEST(TestSuite, Equals)
{
	auto arr1 = Util::makeArray(1, 5, 7);
	ASSERT_TRUE(arr1->equals(arr1));

	auto arr2 = Util::makeArray(1, 5, 7);
	ASSERT_TRUE(arr1->equals(arr2));

	auto arr3 = Util::makeArray(1, 1, 7);
	ASSERT_FALSE(arr1->equals(arr3));
	ASSERT_FALSE(arr3->equals(arr1));

	auto arr4 = Util::makeArray(1, 5);
	ASSERT_FALSE(arr1->equals(arr4));
	ASSERT_FALSE(arr4->equals(arr1));

	auto arr5 = Util::makeArray(1, 5, 7, 9);
	ASSERT_FALSE(arr1->equals(arr5));
	ASSERT_FALSE(arr5->equals(arr1));
}

TEST(TestSuite, AddAll)
{
	auto arr1 = Util::makeArray(1, 2, 3);
	arr1->addAll(Util::makeArray(4, 5));
	// simple add all
	ASSERT_TRUE(arr1->equals(Util::makeArray(1, 2, 3, 4, 5)));

	// add itself
	arr1->addAll(arr1);
	ASSERT_TRUE(arr1->equals(Util::makeArray(1, 2, 3, 4, 5, 1, 2, 3, 4, 5)));
}