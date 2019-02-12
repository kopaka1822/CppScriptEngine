#include "pch.h"
#include "script/Tokenizer.h"

#define TestSuite TokenizerTest
using namespace script;

TEST(TestSuite, L1Test)
{
	auto arr = Tokenizer::getL1Tokens("myObj = other");
	ASSERT_EQ(arr.size(), size_t(3));
	EXPECT_EQ(arr[0].getType(), L1Token::Type::Identifier);
	EXPECT_EQ(arr[1].getType(), L1Token::Type::Assign);
	EXPECT_EQ(arr[2].getType(), L1Token::Type::Identifier);

	arr = Tokenizer::getL1Tokens("myObj = other.func(a, b+c)");
	ASSERT_EQ(arr.size(), size_t(12));
	EXPECT_EQ(arr[0].getType(), L1Token::Type::Identifier);
	EXPECT_EQ(arr[1].getType(), L1Token::Type::Assign);
	EXPECT_EQ(arr[2].getType(), L1Token::Type::Identifier);
	EXPECT_EQ(arr[3].getType(), L1Token::Type::Dot);
	EXPECT_EQ(arr[4].getType(), L1Token::Type::Identifier);
	EXPECT_EQ(arr[5].getType(), L1Token::Type::BracketOpen);
	EXPECT_EQ(arr[6].getType(), L1Token::Type::Identifier);
	EXPECT_EQ(arr[7].getType(), L1Token::Type::Separator);
	EXPECT_EQ(arr[8].getType(), L1Token::Type::Identifier);
	EXPECT_EQ(arr[9].getType(), L1Token::Type::Plus);
	EXPECT_EQ(arr[10].getType(), L1Token::Type::Identifier);
	EXPECT_EQ(arr[11].getType(), L1Token::Type::BracketClosed);

	arr = Tokenizer::getL1Tokens("str = \" my \\\"string\\\"\".count()");
	ASSERT_EQ(arr.size(), size_t(7));
	EXPECT_EQ(arr[0].getType(), L1Token::Type::Identifier);
	EXPECT_EQ(arr[1].getType(), L1Token::Type::Assign);
	EXPECT_EQ(arr[2].getType(), L1Token::Type::String);
	EXPECT_EQ(arr[3].getType(), L1Token::Type::Dot);
	EXPECT_EQ(arr[4].getType(), L1Token::Type::Identifier);
	EXPECT_EQ(arr[5].getType(), L1Token::Type::BracketOpen);
	EXPECT_EQ(arr[6].getType(), L1Token::Type::BracketClosed);
	ASSERT_EQ(arr[2].getValue(), std::string(" my \\\"string\\\""));

	arr = Tokenizer::getL1Tokens("a = 124.1f");
	ASSERT_EQ(arr.size(), size_t(5));
	EXPECT_EQ(arr[0].getType(), L1Token::Type::Identifier);
	EXPECT_EQ(arr[1].getType(), L1Token::Type::Assign);
	EXPECT_EQ(arr[2].getType(), L1Token::Type::Identifier);
	EXPECT_EQ(arr[3].getType(), L1Token::Type::Dot);
	EXPECT_EQ(arr[4].getType(), L1Token::Type::Identifier);

	arr = Tokenizer::getL1Tokens("[3,1.2]");
	ASSERT_EQ(arr.size(), size_t(7));
	EXPECT_EQ(arr[0].getType(), L1Token::Type::ArrayOpen);
	EXPECT_EQ(arr[1].getType(), L1Token::Type::Identifier);
	EXPECT_EQ(arr[2].getType(), L1Token::Type::Separator);
	EXPECT_EQ(arr[3].getType(), L1Token::Type::Identifier);
	EXPECT_EQ(arr[4].getType(), L1Token::Type::Dot);
	EXPECT_EQ(arr[5].getType(), L1Token::Type::Identifier);
	EXPECT_EQ(arr[6].getType(), L1Token::Type::ArrayClosed);

	arr = Tokenizer::getL1Tokens("-3.12f/4+3*99");
	ASSERT_EQ(arr.size(), size_t(10));
	EXPECT_EQ(arr[0].getType(), L1Token::Type::Minus);
	EXPECT_EQ(arr[1].getType(), L1Token::Type::Identifier);
	EXPECT_EQ(arr[2].getType(), L1Token::Type::Dot);
	EXPECT_EQ(arr[3].getType(), L1Token::Type::Identifier);
	EXPECT_EQ(arr[4].getType(), L1Token::Type::Divide);
	EXPECT_EQ(arr[5].getType(), L1Token::Type::Identifier);
	EXPECT_EQ(arr[6].getType(), L1Token::Type::Plus);
	EXPECT_EQ(arr[7].getType(), L1Token::Type::Identifier);
	EXPECT_EQ(arr[8].getType(), L1Token::Type::Multiply);
	EXPECT_EQ(arr[9].getType(), L1Token::Type::Identifier);
	EXPECT_EQ(arr[9].getValue(), std::string("99"));
}

TEST(TestSuite, FinalizeL1Test)
{
	// check number and function conversion
	auto tokens = Tokenizer::getL1Tokens("a342234(3.1f,.5f,123f,30)");
	Tokenizer::finalizeL1Tokens(tokens);
	ASSERT_EQ(tokens.size(), 9);
	EXPECT_EQ(tokens[0].getType(), L1Token::Type::Function);
	EXPECT_EQ(tokens[0].getValue(), std::string("a342234"));
	EXPECT_EQ(tokens[1].getType(), L1Token::Type::Float);
	EXPECT_EQ(tokens[1].getValue(), std::string("3.1f"));
	EXPECT_EQ(tokens[2].getType(), L1Token::Type::Separator);
	EXPECT_EQ(tokens[3].getType(), L1Token::Type::Float);
	EXPECT_EQ(tokens[4].getType(), L1Token::Type::Separator);
	EXPECT_EQ(tokens[5].getType(), L1Token::Type::Float);
	EXPECT_EQ(tokens[6].getType(), L1Token::Type::Separator);
	EXPECT_EQ(tokens[7].getType(), L1Token::Type::Integer);
	EXPECT_EQ(tokens[8].getType(), L1Token::Type::BracketClosed);

	// test for other constants
	tokens = Tokenizer::getL1Tokens("[true, false, null, v23]");
	Tokenizer::finalizeL1Tokens(tokens);
	ASSERT_EQ(tokens.size(), 9);
	EXPECT_EQ(tokens[0].getType(), L1Token::Type::ArrayOpen);
	EXPECT_EQ(tokens[1].getType(), L1Token::Type::Bool);
	EXPECT_EQ(tokens[2].getType(), L1Token::Type::Separator);
	EXPECT_EQ(tokens[3].getType(), L1Token::Type::Bool);
	EXPECT_EQ(tokens[4].getType(), L1Token::Type::Separator);
	EXPECT_EQ(tokens[5].getType(), L1Token::Type::Null);
	EXPECT_EQ(tokens[6].getType(), L1Token::Type::Separator);
	EXPECT_EQ(tokens[7].getType(), L1Token::Type::Identifier);
	EXPECT_EQ(tokens[8].getType(), L1Token::Type::ArrayClosed);

	// test for invalid types
	tokens = Tokenizer::getL1Tokens("1func()");
	ASSERT_THROW(Tokenizer::finalizeL1Tokens(tokens), SyntaxError);

	tokens = Tokenizer::getL1Tokens("3a33f");
	ASSERT_THROW(Tokenizer::finalizeL1Tokens(tokens), SyntaxError);

	tokens = Tokenizer::getL1Tokens("342f.4f");
	Tokenizer::finalizeL1Tokens(tokens);
	ASSERT_EQ(tokens.size(), 2);
	EXPECT_EQ(tokens[0].getType(), L1Token::Type::Float);
	EXPECT_EQ(tokens[0].getValue(), std::string("342f"));
	EXPECT_EQ(tokens[1].getType(), L1Token::Type::Float);
}