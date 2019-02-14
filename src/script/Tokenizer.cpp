#include "../../include/script/Tokenizer.h"
#include <regex>
#include <array>
#include "script/tokens/L1Rule.h"
#include <stack>
#include <cassert>

std::unique_ptr<script::L2Token> script::Tokenizer::getExecuteable(const std::string& command)
{
	std::vector<L1Token> tokens = getL1Tokens(command);
	applyL1Rules(tokens);
	verifyBrackets(tokens);

	auto start = tokens.cbegin();
	auto res = getL2Tokens(start, tokens.cend(), false);
	if (start != tokens.cend())
		throw SyntaxError(start->getPosition(), start->getValue(), "");

	return res;
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
		L1Token curToken = L1Token(L1Token::Type::Undefined, 0, "");
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
			curToken = L1Token(L1Token::Type::Assign, position, "=");
			break;
		case '.':
			curToken = L1Token(L1Token::Type::Dot, position, ".");
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
			curToken = L1Token(L1Token::Type::Separator, position, ",");
			break;
		case '(':
			curToken = L1Token(L1Token::Type::BracketOpen, position, "(");
			break;
		case ')':
			curToken = L1Token(L1Token::Type::BracketClosed, position, ")");
			break;
		case '[':
			curToken = L1Token(L1Token::Type::ArrayOpen, position, "[");
			break;
		case ']':
			curToken = L1Token(L1Token::Type::ArrayClosed, position, "]");
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

std::unique_ptr<script::L2Token> script::Tokenizer::getL2Tokens(std::vector<L1Token>::const_iterator& start, std::vector<L1Token>::const_iterator end, bool isArgumentList)
{
	std::unique_ptr<L2Token> curToken;
	while (start != end)
	{
		switch (start->getType())
		{
			// primitives
		case L1Token::Type::String:
			if (curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "");

			curToken = std::make_unique<L2PrimitiveValueToken<std::string>>(start++->getValue());
		case L1Token::Type::Float:
			if (curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "");

			curToken = std::make_unique<L2PrimitiveValueToken<float>>(start++->getFloatValue());
		case L1Token::Type::Integer:
			if (curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "");

			curToken = std::make_unique<L2PrimitiveValueToken<int>>(start++->getIntValue());
		case L1Token::Type::Bool:
			if (curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "");

			curToken = std::make_unique<L2PrimitiveValueToken<bool>>(start++->getBoolValue());
		case L1Token::Type::Null:
			if (curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "");

			++start;
			curToken = std::make_unique<L2PrimitiveValueToken<nullptr_t>>(nullptr);
		case L1Token::Type::Identifier: {
			if (curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "");

			const auto val = start++;
			curToken = std::make_unique<L2IdentifierToken>(val->getValue(), val->getPosition());
		}
		case L1Token::Type::Assign:
			// not supported yet... replace with identifier assign
			throw SyntaxError(start->getPosition(), start->getValue(), "");

		case L1Token::Type::IdentifierAssign: {
			if (curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "");

			auto name = start->getValue();
			auto val = getL2Tokens(++start, end, true);
			return std::make_unique<L2IdentifierAssignToken>(name, move(val));
		}

		case L1Token::Type::Separator:
			if (isArgumentList) 
				break;
			throw SyntaxError(start->getPosition(), start->getValue(), "separator is only valid within a function or array");

		case L1Token::Type::BracketOpen: {
			if (curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "");

			// read value within brackets
			const auto pos = start->getPosition();
			curToken = getL2Tokens(++start, end, true);
			// start should point to bracket end
			if (start == end) 
				throw SyntaxError(pos, "end of command", "missing closing bracket");
			if (start->getType() != L1Token::Type::BracketClosed)
				throw BracketMismatch(start->getPosition(), ")",start->getValue());
			// everything is fine
			++start;
		}

		case L1Token::Type::ArrayClosed:
		case L1Token::Type::BracketClosed:
			if (isArgumentList)
				break;
			throw SyntaxError(start->getPosition(), start->getValue(), "");
		
		case L1Token::Type::ArrayOpen: {
			if (curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "");
			// parse arguments
			auto args = parseArgumentList(++start, end, L1Token::Type::ArrayClosed, "array");
			curToken = move(args);
		} break;

		case L1Token::Type::Dot: {
			if (!curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "function call needs a valid object");
			auto pos = start->getPosition();
			++start;
			if (start == end)
				throw SyntaxError(pos, "end of command", "missing function call after \".\"");
			if (start->getType() != L1Token::Type::Function)
				throw SyntaxError(start->getPosition(), start->getValue(), "expected function call");
			auto funcName = start->getValue();
			// parse arguments
			auto args = parseArgumentList(++start, end, L1Token::Type::BracketClosed, "function call");

			// function call ended
			curToken = std::make_unique<L2FunctionToken>(move(curToken), funcName, pos, move(args));
		}break;

		case L1Token::Type::Function:
			// function call without a preceding dot (maybe support that later?)
			throw SyntaxError(start->getPosition(), start->getValue(), "functions call must be preceded by a dot");
		default: ;
		}
	}

	if (!curToken)
	{
		if(start == end)
			throw SyntaxError(-1, "end of command", "");
		throw SyntaxError(start->getPosition(), start->getValue(), "could not parse a value prior to this token");
	}

	return curToken;
}

std::unique_ptr<script::L2ArgumentListToken> script::Tokenizer::parseArgumentList(
	std::vector<L1Token>::const_iterator& start, std::vector<L1Token>::const_iterator end, L1Token::Type endToken, const std::string& type)
{
	auto args = std::make_unique<L2ArgumentListToken>();

	if (start == end)
		throw SyntaxError(-1, "end of command", type + " was not closed");
	if (start->getType() == endToken)
	{
		++start;
		return args;
	}

	while (true)
	{
		if (start == end)
			throw SyntaxError(-1, "end of command", type + " was not closed");

		auto value = getL2Tokens(++start, end, true);
		args->add(move(value));

		if (start == end)
			throw SyntaxError(-1, "end of command", type + " was not closed");
		if (start->getType() == endToken)
			break; // function call end
		if (start->getType() != L1Token::Type::Separator)
			throw SyntaxError(start->getPosition(), start->getValue(), type + " not closed. expected either \",\" or \"=\"");
		++start;
	}

	++start;
	return args;
}
