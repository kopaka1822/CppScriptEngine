#include "pch.h"

#define TestSuite ValueObjectTest
using namespace script;



TEST(TestSuite, DefaultConstructor)
{
	class SimpleClass
	{
	public:
		int a = 3;
		int b = 4;
	};

	class SimpleClassWrapper : public ValueObject<SimpleClass>
	{
	public:
		SimpleClassWrapper()
			:
		ValueObject()
		{}

		std::string toString() const override { return ""; }
		ScriptObjectPtr clone() const override { return nullptr; }
	};

	auto simpleObj = std::make_shared<SimpleClassWrapper>();
	ASSERT_TRUE(simpleObj);
	ASSERT_EQ(simpleObj->getValue().a, 3);
	ASSERT_EQ(simpleObj->getValue().b, 4);
}

TEST(TestSuite, MoveConstructor)
{
	class OnlyMoveableClass
	{
	public:
		OnlyMoveableClass() = default;
		OnlyMoveableClass(const OnlyMoveableClass&) = delete;
		OnlyMoveableClass& operator=(const OnlyMoveableClass&) = delete;
		OnlyMoveableClass(OnlyMoveableClass&& m) noexcept : a(m.a) {}
		OnlyMoveableClass& operator==(OnlyMoveableClass&& m)
		{
			a = m.a;
			return *this;
		}

		int a = 10;
	};

	class OnlyMoveableClassWrapper : public ValueObject<OnlyMoveableClass>
	{
	public:
		OnlyMoveableClassWrapper() = default;
		OnlyMoveableClassWrapper(OnlyMoveableClass&& m)
			:
			ValueObject<OnlyMoveableClass>(std::move(m))
		{}

		std::string toString() const override { return ""; }
		ScriptObjectPtr clone() const override { return nullptr; }
	};

	auto moveObj = std::make_shared<OnlyMoveableClassWrapper>();
	ASSERT_TRUE(moveObj);
	ASSERT_EQ(moveObj->getValue().a, 10);

	OnlyMoveableClass inst;
	inst.a = 20;
	moveObj = std::make_shared<OnlyMoveableClassWrapper>(std::move(inst));
	ASSERT_TRUE(moveObj);
	ASSERT_EQ(moveObj->getValue().a, 20);
}

TEST(TestSuite, InplaceConstructor)
{
	class InPlaceClass
	{
	public:
		InPlaceClass() = delete;
		InPlaceClass(const InPlaceClass&) = delete;
		InPlaceClass& operator=(const InPlaceClass&) = delete;
		InPlaceClass(InPlaceClass&& m) = delete;
		InPlaceClass& operator==(InPlaceClass&& m) = delete;
		InPlaceClass(std::string a, int b) : a(move(a)), b(b) {}

		const std::string a;
		const int b;
	};

	class InPlaceClassWrapper : public ValueObject<InPlaceClass>
	{
	public:
		InPlaceClassWrapper(std::string a, int b) 
			:
		ValueObject<InPlaceClass>(std::move(a), b)
		{}

		std::string toString() const override { return ""; }
		ScriptObjectPtr clone() const override { return nullptr; }
	};

	auto placeObj = std::make_shared<InPlaceClassWrapper>("Test", 12);
	ASSERT_TRUE(placeObj);
	ASSERT_EQ(placeObj->getValue().a, "Test");
	ASSERT_EQ(placeObj->getValue().b, 12);
}