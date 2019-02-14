#include "../../include/script/Tokenizer.h"
#include <regex>
#include <array>
#include "script/tokens/L1Rule.h"
#include <stack>

std::vector<script::L1Token> script::Tokenizer::getTokens(const std::string& command)
{
	std::vector<L1Token> tokens = getL1Tokens(command);
	applyL1Rules(tokens);
	verifyBrackets(tokens);

	return tokens;
}

std::vector<script::L1Token> script::Tokenizer::getL1Tokens(const std::string& command)
{
	std::vector<L1Token> tokens;
	tokens.reserve(100);

	// indicates if a string is being parsed
	bool isString = false;
	std::string current;
	auto position = size_t(-1);

	// parse tokens
	for (auto& c : command)
	{
		++position;

		// new token
		L1Token curToken = L1Token(L1Token::Type::Undefined, 0);
		if (isString)
		{
			if (c != '\"' || // no closing " or
				(!current.empty() && current.back() == '\\')) // previous character was a '\'
			{
				current += c;
				continue;
			}

			// close string
			isString = false;
			curToken = L1Token(L1Token::Type::String, position - current.length(), current);
			current.resize(0);
		}
		else switch (c) // no string
		{
		case '\"':
			isString = true;
			break;
		case '=':
			curToken = L1Token(L1Token::Type::Assign, position);
			break;
		case '.':
			curToken = L1Token(L1Token::Type::Dot, position);
			break;
		//case '+':
		//	curToken = L1Token(L1Token::Type::Plus, position);
		//	break;
		//case '-':
		//	curToken = L1Token(L1Token::Type::Minus, position);
		//	break;
		//case '*':
		//	curToken = L1Token(L1Token::Type::Multiply, position);
		//	break;
		//case '/':
		//	curToken = L1Token(L1Token::Type::Divide, position);
		//	break;
		case ',':
			curToken = L1Token(L1Token::Type::Separator, position);
			break;
		case '(':
			curToken = L1Token(L1Token::Type::BracketOpen, position);
			break;
		case ')':
			curToken = L1Token(L1Token::Type::BracketClosed, position);
			break;
		case '[':
			curToken = L1Token(L1Token::Type::ArrayOpen, position);
			break;
		case ']':
			curToken = L1Token(L1Token::Type::ArrayClosed, position);
			break;
		default:
			if (!isspace(static_cast<unsigned char>(c)))
			{
				if (!isalnum(static_cast<unsigned char>(c)))
					throw SyntaxError(position, std::string(&c, 1), "variables and functions must be alphanumeric");
				current += c;
				continue;
			}

		}

		// make token from string
		if (current.length())
		{
			tokens.emplace_back(L1Token::Type::Identifier, position - current.length(), current);
			current.resize(0);
		}

		if (curToken.getType() != L1Token::Type::Undefined)
		{
			// add token and clear current string
			tokens.push_back(curToken);
		}
	}

	if (current.length())
		tokens.emplace_back(L1Token::Type::Identifier, position - current.length(), current);

	return tokens;
}

void script::Tokenizer::applyL1Rules(std::vector<L1Token>& tokens)
{
	static const L1FloatRule1 floatRule1;
	static const L1FloatRule2 floatRule2;
	static const L1NumberRule numberRule;
	static const L1FunctionRule functionRule;
	//static const L1OperatorAssignRule operatorAssignRule;
	static const L1IdentifierAssignRule idAssignRule;
	static const std::array<const L1Rule*, 5> rules = {
		&floatRule1, &floatRule2,&numberRule, 
		&functionRule, /*&operatorAssignRule,*/ &idAssignRule
	};

	// apply rules in order
	for(const auto& rule : rules)
	{
		// traverse tokens from right to left
		for(int i = tokens.size() - rule->patternLength(); i >= 0; --i)
		{
			if (rule->matches(tokens.begin() + i))
			{
				tokens[i] = rule->apply(tokens.begin() + i);
				// remove other tokens
				tokens.erase(tokens.begin() + i + 1, tokens.begin() + i + rule->patternLength());
			}
		}
	}
}

void script::Tokenizer::verifyBrackets(const std::vector<L1Token>& tokens)
{
	// test if  brackets are correctly set
	std::stack<L1Token::Type> brackets;
	for (const auto& t : tokens)
	{
		switch (t.getType())
		{
		case L1Token::Type::Separator:
			// this does only make sense inside a function or array
			if (brackets.empty())
				throw SyntaxError(t.getPosition(), ",", "Separator is only allowed within a function or array");
			break;
		case L1Token::Type::BracketOpen:
			brackets.push(t.getType());
			break;
		case L1Token::Type::BracketClosed:
			if (brackets.empty())
				throw SyntaxError(t.getPosition(), ")", "all brackets were closed");
			if (brackets.top() != L1Token::Type::BracketOpen && brackets.top() != L1Token::Type::Function)
				throw BracketMismatch(t.getPosition(), "]", ")");
			brackets.pop();
			break;

		case L1Token::Type::ArrayOpen:
			brackets.push(t.getType());
			break;
		case L1Token::Type::ArrayClosed:
			if (brackets.empty())
				throw SyntaxError(t.getPosition(), ")", "all brackets were closed");
			if (brackets.top() != L1Token::Type::ArrayClosed)
				throw BracketMismatch(t.getPosition(), ")", "]");
			break;
		}
	}
}

std::vector<std::unique_ptr<script::L2Token>> script::Tokenizer::getL2Tokens(const std::vector<L1Token>& tokens)
{
	std::vector<std::unique_ptr<L2Token>> res;
	for (auto t = tokens.begin(), end = tokens.end(); t != end;)
	{
		switch (t->getType())
		{
			// primitives
		case L1Token::Type::String: 
			res.emplace_back(std::make_unique<PrimitiveValueToken<std::string>>(t->getValue()));
			break;
		case L1Token::Type::Float: 
			res.emplace_back(std::make_unique<PrimitiveValueToken<float>>(t->getFloatValue()));
			break;
		case L1Token::Type::Integer: 
			res.emplace_back(std::make_unique<PrimitiveValueToken<int>>(t->getIntValue()));
			break;
		case L1Token::Type::Bool: 
			res.emplace_back(std::make_unique<PrimitiveValueToken<bool>>(t->getBoolValue()));
			break;
		case L1Token::Type::Null: 
			res.emplace_back(std::make_unique<PrimitiveValueToken<nullptr_t>>(nullptr));
			break;
		case L1Token::Type::Identifier:
			res.emplace_back(std::make_unique<IdentifierToken>(t->getValue(), t->getPosition()));
			break;

		case L1Token::Type::Assign: 
			break;

		case L1Token::Type::Separator: break;
		case L1Token::Type::BracketOpen: break;
		case L1Token::Type::BracketClosed: break;
		
		case L1Token::Type::ArrayOpen: break;
		case L1Token::Type::ArrayClosed: break;

		case L1Token::Type::Dot: 
		
			break;

		case L1Token::Type::Function: break;
		default: ;
		}

		++t;
	}

	return res;
}
