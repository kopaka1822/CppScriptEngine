#include "../../include/script/Tokenizer.h"
#include "../../include/script/tokens/L1FloatRule1.h"
#include "../../include/script/tokens/L1FloatRule2.h"
#include "../../include/script/tokens/L1NumberRule.h"
#include "../../include/script/tokens/L1FunctionRule.h"
#include "../../include/script/tokens/L1StaticFunctionRule.h"
#include "../../include/script/tokens/L1OperatorAssignRule.h"
#include "../../include/script/tokens/L1IdentifierAssignRule.h"
#include "../../include/script/tokens/L2ArgumentListToken.h"
#include "../../include/script/tokens/L2FunctionToken.h"
#include "../../include/script/tokens/L2StaticFunctionToken.h"
#include "../../include/script/tokens/L2IdentifierAssignToken.h"
#include "../../include/script/tokens/L2IdentifierToken.h"
#include "../../include/script/tokens/L2OperatorToken.h"
#include "../../include/script/tokens/L2PropertyGetterToken.h"
#include "../../include/script/tokens/L2PropertySetterToken.h"
#include "../../include/script/tokens/L2StaticIdentifierToken.h"
#include "../../include/script/tokens/L2PrimitiveValueToken.h"
#include "../../include/script/objects/FloatObject.h"
#include <array>
#include <stack>

std::unique_ptr<script::L2Token> script::Tokenizer::getExecutable(const std::string& command)
{
	std::vector<L1Token> tokens = getL1Tokens(command);
	verifyBrackets(tokens);
	applyL1Rules(tokens);

	auto start = tokens.cbegin();
	auto res = getL2Tokens(start, tokens.cend(), false, OpReturnMode::End);
	if (start != tokens.cend())
		throw SyntaxError(start->getPosition(), start->getValue(), "");

	return res;
}

script::Tokenizer::AutocompleteInfo script::Tokenizer::getAutocomplete(const std::string& command)
{
	std::vector<L1Token> tokens;
	try
	{
		tokens = getL1Tokens(command);
		applyL1Rules(tokens);
	}
	catch (const std::exception&){}

	AutocompleteInfo i;
	// take a look at the last token
	if(tokens.empty())
	{
		// everything is possible
		return i;
	}

	if (isspace(command.back()))
	{
		// , ) something else? but no variable
		i.noProposal = true;
		return i;
	}

	switch (tokens.back().getType()) 
	{ 
	case L1Token::Type::Assign:
	case L1Token::Type::Separator:
	case L1Token::Type::BracketOpen:
	case L1Token::Type::ArrayOpen:
	case L1Token::Type::Plus:
	case L1Token::Type::PlusAssign: 
	case L1Token::Type::Minus:
	case L1Token::Type::MinusAssign:
	case L1Token::Type::Multiply:
	case L1Token::Type::MultiplyAssign:
	case L1Token::Type::Divide:
	case L1Token::Type::DivideAssign:
	case L1Token::Type::IdentifierAssign:
	case L1Token::Type::StaticFunction:
	case L1Token::Type::Function:
		// everything is possible
		return i;

	case L1Token::Type::OpenString:
		i.objectToken = tokens.back();
		return i;
	
	case L1Token::Type::Undefined:
	case L1Token::Type::BracketClosed:
	case L1Token::Type::ArrayClosed:
	case L1Token::Type::Null:
	case L1Token::Type::Float:
	case L1Token::Type::Bool:
	case L1Token::Type::Integer:
	case L1Token::Type::String:
		// no variables => , ) . are possible
		i.noProposal = true;
		return i;

	case L1Token::Type::Identifier:
		// complete identifier
		i.objectToken = tokens.back();
		fillAutocompleteCaller(tokens, i);
		return i;

	case L1Token::Type::Dot: 
		// what was the previous token?
		fillAutocompleteCaller(tokens, i);
		return i;
	default: 
		// what happened?
		i.noProposal = true;
		return i;
	}
}

std::vector<script::L1Token> script::Tokenizer::getL1Tokens(const std::string& command, bool throwExceptions)
{
	std::vector<L1Token> tokens;
	tokens.reserve(100);

	// indicates if a string is being parsed
	bool isString = false;
	// indicates if the current character was an escape character
	bool curEscaped = false;
	// indicates if the previous character was an escape character
	bool prevEscaped = false;
	std::string current;
	auto position = size_t(-1);

	// parse tokens
	for (auto& c : command)
	{
		++position;

		prevEscaped = curEscaped;
		curEscaped = c == '\\';

		// new token
		L1Token curToken = L1Token(L1Token::Type::Undefined, 0, "");
		if (isString)
		{
			if (curEscaped) continue;

			if(prevEscaped)
			{
				current += getEscapedChar(position, c);
				continue;
			}

			if (c != '\"')
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
		case '+':
			curToken = L1Token(L1Token::Type::Plus, position, "+");
			break;
		case '-':
			curToken = L1Token(L1Token::Type::Minus, position, "-");
			break;
		case '*':
			curToken = L1Token(L1Token::Type::Multiply, position, "*");
			break;
		case '/':
			curToken = L1Token(L1Token::Type::Divide, position, "/");
			break;
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
				if (throwExceptions && !isalnum(static_cast<unsigned char>(c)))
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

	if (isString)
	{
		if(throwExceptions)
			throw SyntaxError(-1, "end of command", "string not closed");

		tokens.emplace_back(L1Token::Type::OpenString, position, current);
	}
	else if (current.length())
		tokens.emplace_back(L1Token::Type::Identifier, position - current.length(), current);

	return tokens;
}

void script::Tokenizer::applyL1Rules(std::vector<L1Token>& tokens)
{
	static const L1FloatRule1 floatRule1;
	static const L1FloatRule2 floatRule2;
	static const L1NumberRule numberRule;
	static const L1FunctionRule functionRule1;
	static const L1StaticFunctionRule functionRule2;
	static const L1OperatorAssignRule operatorAssignRule;
	static const L1IdentifierAssignRule idAssignRule;
	static const std::array<const L1Rule*, 7> rules = {
		&floatRule1, &floatRule2,&numberRule, 
		&functionRule1, &functionRule2, 
		&operatorAssignRule, &idAssignRule
	};

	// apply rules in order
	for(const auto& rule : rules)
	{
		// traverse tokens from right to left
		for(int i = int(tokens.size()) - rule->patternLength(); i >= 0; --i)
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
	std::stack<L1Token> expectedBracket;
	for (const auto& t : tokens)
	{
		switch (t.getType())
		{
		case L1Token::Type::Separator:
			// this does only make sense inside a function or array
			if (expectedBracket.empty())
				throw SyntaxError(t.getPosition(), ",", "Separator is only allowed within a function or array");
			break;
		case L1Token::Type::BracketOpen:
			expectedBracket.push(L1Token(L1Token::Type::BracketClosed, t.getPosition(), ")"));
			break;
		case L1Token::Type::ArrayOpen:
			expectedBracket.push(L1Token(L1Token::Type::ArrayClosed, t.getPosition(), "]"));
			break;

		case L1Token::Type::BracketClosed:
		case L1Token::Type::ArrayClosed:
			if (expectedBracket.empty())
				throw SyntaxError(t.getPosition(), t.getValue(), "all brackets were closed");
			if (expectedBracket.top().getType() != t.getType())
				throw BracketMismatch(t.getPosition(), expectedBracket.top().getValue(), t.getValue());
			expectedBracket.pop();
			break;
		}
	}

	if (!expectedBracket.empty())
		throw SyntaxError(-1, "end of command", std::to_string(expectedBracket.size()) + " unclosed brackets");
}

std::unique_ptr<script::L2Token> script::Tokenizer::getL2Tokens(std::vector<L1Token>::const_iterator& start, std::vector<L1Token>::const_iterator end,
	bool isArgumentList, OpReturnMode mode)
{
	std::unique_ptr<L2Token> curToken;
	while (start != end)
	{
		switch (start->getType())
		{
#pragma region Primitives
		case L1Token::Type::String:
			if (curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "");

			curToken = std::make_unique<L2PrimitiveValueToken<std::string>>(start++->getValue());
			break;
		case L1Token::Type::Float:
			if (curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "");

			curToken = std::make_unique<L2PrimitiveValueToken<float>>(start++->getFloatValue());
			break;
		case L1Token::Type::Integer:
			if (curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "");

			curToken = std::make_unique<L2PrimitiveValueToken<int>>(start++->getIntValue());
			break;
		case L1Token::Type::Bool:
			if (curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "");

			curToken = std::make_unique<L2PrimitiveValueToken<bool>>(start++->getBoolValue());
			break;
		case L1Token::Type::Null:
			if (curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "");

			++start;
			curToken = std::make_unique<L2PrimitiveValueToken<nullptr_t>>(nullptr);
			break;
#pragma endregion

#pragma region Identifier
		case L1Token::Type::Identifier: {
			if (curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "");

			const auto val = start++;
			if (val->startsWithLowercase())
				curToken = std::make_unique<L2IdentifierToken>(val->getValue(), val->getPosition());
			else
				curToken = std::make_unique<L2StaticIdentifierToken>(val->getValue(), val->getPosition());
		}   break;

		case L1Token::Type::IdentifierAssign: {
			if (curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "");

			auto name = start->getValue();
			auto val = getL2Tokens(++start, end, true, OpReturnMode::End);
			return std::make_unique<L2IdentifierAssignToken>(name, move(val));
		}
#pragma endregion

#pragma region Assign
		case L1Token::Type::Undefined:
		case L1Token::Type::Assign:
			// not supported yet... replace with identifier assign
			throw SyntaxError(start->getPosition(), start->getValue(), "");
		case L1Token::Type::PlusAssign: 
			handleOperatorAssign(start, end, curToken, isArgumentList, "add", "+=");
			break;
		case L1Token::Type::MinusAssign:
			handleOperatorAssign(start, end, curToken, isArgumentList, "subtract", "-=");
			break;
		case L1Token::Type::MultiplyAssign:
			handleOperatorAssign(start, end, curToken, isArgumentList, "multiply", "*=");
			break;
		case L1Token::Type::DivideAssign:
			handleOperatorAssign(start, end, curToken, isArgumentList, "divide", "/=");
			break;
#pragma endregion

#pragma region Array and Brackets
		case L1Token::Type::Separator:
			if (isArgumentList) 
				goto loopend;
			throw SyntaxError(start->getPosition(), start->getValue(), "separator is only valid within a function or array");

		case L1Token::Type::BracketOpen: {
			if (curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "");

			// read value within brackets
			const auto pos = start->getPosition();
			curToken = getL2Tokens(++start, end, true, OpReturnMode::End);
			// start should point to bracket end
			if (start == end) 
				throw SyntaxError(pos, "end of command", "missing closing bracket");
			if (start->getType() != L1Token::Type::BracketClosed)
				throw BracketMismatch(start->getPosition(), ")",start->getValue());
			// everything is fine
			++start;
			break;
		}

		case L1Token::Type::ArrayClosed:
		case L1Token::Type::BracketClosed:
			if (isArgumentList)
				goto loopend;  // NOLINT(cppcoreguidelines-avoid-goto, hicpp-avoid-goto)
			throw SyntaxError(start->getPosition(), start->getValue(), "");
		
		case L1Token::Type::ArrayOpen: {
			if (curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "");
			// parse arguments
			auto args = parseArgumentList(++start, end, L1Token::Type::ArrayClosed, "array");
			curToken = move(args);
		} break;

#pragma endregion 

#pragma region Operators
		case L1Token::Type::Plus: {			
			auto pos = start->getPosition();
			if (!curToken) // ignore (would no change the sign of the object)
			{
				++start;
				break;
			}
			if (mode == OpReturnMode::PlusMinus || mode == OpReturnMode::MultDiv)
				return curToken;

			// add curToken with the next value
			auto nextVal = getL2Tokens(++start, end, isArgumentList, OpReturnMode::PlusMinus);
			curToken = std::make_unique<L2OperatorToken>(move(curToken), move(nextVal), pos, "add", true, "+");
		} break;
		case L1Token::Type::Minus:
			if(!curToken)
			{
				// changes the sign of the next object
				auto pos = start->getPosition();
				auto nextVal = getL2Tokens(++start, end, isArgumentList, OpReturnMode::MultDiv);
				curToken = std::make_unique<L2OperatorToken>(move(nextVal), nullptr, pos, "negate", true, "-");
			}
			else
			{
				// subtract with the next object
				if (mode == OpReturnMode::PlusMinus || mode == OpReturnMode::MultDiv)
					return curToken;

				auto pos = start->getPosition();
				auto nextVal = getL2Tokens(++start, end, isArgumentList, OpReturnMode::PlusMinus);
				curToken = std::make_unique<L2OperatorToken>(move(curToken), move(nextVal), pos, "subtract", true, "-");
			}
			break;
		case L1Token::Type::Multiply: {
			if (!curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "operand on left side missing");
			if (mode == OpReturnMode::MultDiv)
				return curToken;
			auto pos = start->getPosition();
			// multiply with next value
			auto nextVal = getL2Tokens(++start, end, isArgumentList, OpReturnMode::MultDiv);
			curToken = std::make_unique<L2OperatorToken>(move(curToken), move(nextVal), pos, "multiply", true, "*");
		}break;
		case L1Token::Type::Divide: {
			if (!curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "operand on left side missing");
			if (mode == OpReturnMode::MultDiv)
				return curToken;
			auto pos = start->getPosition();
			// divide with next value
			auto nextVal = getL2Tokens(++start, end, isArgumentList, OpReturnMode::MultDiv);
			curToken = std::make_unique<L2OperatorToken>(move(curToken), move(nextVal), pos, "divide", true, "/");
		}break;
#pragma endregion 

#pragma region Function Call
		case L1Token::Type::Dot: {
			if (!curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "function call needs a valid object");
			auto pos = start->getPosition();
			++start;
			if (start == end)
				throw SyntaxError(pos, "end of command", "missing function call after \".\"");

			if(start->getType() == L1Token::Type::Function)
			{
				// make function call
				auto funcName = start->getValue();
				// parse arguments
				auto args = parseArgumentList(++start, end, L1Token::Type::BracketClosed, "function call");

				// function call ended
				curToken = std::make_unique<L2FunctionToken>(move(curToken), funcName, pos, move(args));
			}
			else if(start->getType() == L1Token::Type::Identifier)
			{
				// handle getter
				if (!start->startWithUppercase())
					throw SyntaxError(pos, start->getValue(), "property token must start with an uppercase letter");

				// make getter function
				curToken = std::make_unique<L2PropertyGetterToken>(move(curToken), start->getValue(), pos);
				++start;
			}
			else if(start->getType() == L1Token::Type::IdentifierAssign)
			{
				// handle setter
				if (!start->startWithUppercase())
					throw SyntaxError(pos, start->getValue(), "property token must start with an uppercase letter");

				// make setter function
				auto setName = start->getValue();
				auto arg = getL2Tokens(++start, end, false, OpReturnMode::End);
				curToken = std::make_unique<L2PropertySetterToken>(move(curToken), move(arg), move(setName), pos);
			}
			else throw SyntaxError(start->getPosition(), start->getValue(), "expected function call or property");
		} break;

		case L1Token::Type::StaticFunction: {
			if (curToken)
				throw SyntaxError(start->getPosition(), start->getValue(), "");
			auto name = start->getValue();
			auto pos = start->getPosition();
			auto args = parseArgumentList(++start, end, L1Token::Type::BracketClosed, "function call");

			curToken = std::make_unique<L2StaticFunctionToken>(name, pos, move(args));
		} break;

		case L1Token::Type::Function:
			// function call without a preceding dot (maybe support that later?)
			throw SyntaxError(start->getPosition(), start->getValue(), "functions call must be preceded by a dot");

#pragma endregion 
			default: ;
		}
	}
	loopend:

	if (!curToken)
	{
		if(start == end)
			throw SyntaxError(-1, "end of command", "");
		throw SyntaxError(start->getPosition(), start->getValue(), "could not parse a value prior to this token");
	}

	return curToken;
}

std::unique_ptr<script::L2Token> script::Tokenizer::parseArgumentList(
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

		auto value = getL2Tokens(start, end, true, OpReturnMode::End);
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

void script::Tokenizer::handleOperatorAssign(std::vector<L1Token>::const_iterator& start,
	std::vector<L1Token>::const_iterator end, std::unique_ptr<L2Token>& curToken, bool isArgumentList,
	std::string funcName, std::string opName)
{
	if (!curToken)
		throw SyntaxError(start->getPosition(), start->getValue(), "operand on left side missing");

	auto pos = start->getPosition();
	auto nextVal = getL2Tokens(++start, end, isArgumentList, OpReturnMode::End);
	curToken = std::make_unique<L2OperatorToken>(move(curToken), move(nextVal), pos, move(funcName), false, move(opName));
}

char script::Tokenizer::getEscapedChar(size_t position, char value)
{
	switch (value)
	{
	case '\'':
		return '\'';
	case '"':
		return '\"';
	case '?':
		return '\?';
	case '\\':
		return '\\';
	case 'a':
		return '\a';
	case 'b':
		return '\b';
	case 'f':
		return '\f';
	case 'n':
		return '\n';
	case 'r':
		return '\r';
	case 't':
		return '\t';
	case 'v':
		return '\v';
	}
	std::string token = "\\?";
	token[1] = value;
	throw SyntaxError(position, token, "escape sequence not supported");
}

void script::Tokenizer::fillAutocompleteCaller(const std::vector<L1Token>& tokens, AutocompleteInfo& info)
{
	auto it = tokens.rbegin();

	// search the part before the dot
	if(it->getType() != L1Token::Type::Dot)
	{
		++it;
		if (it == tokens.rend()) return; // no function call


		if (it->getType() != L1Token::Type::Dot)
			return; // no function call
	}

	// determine on which object the function is called
	++it;
	if (it == tokens.rend()) return; // invalid syntax?

	switch (it->getType())
	{
#pragma region PrimitveTypes
		// use some dummy object to determine the available functions
	case L1Token::Type::String: 
		info.callerObject = std::make_shared<StringObject>("");
		return;
	case L1Token::Type::Bool: 
		info.callerObject = std::make_shared<BoolObject>(true);
		return;
	case L1Token::Type::Integer: 
		info.callerObject = std::make_shared<IntObject>(1);
		return;
	case L1Token::Type::Null: 
		info.callerObject = NullObject::get();
		return;
	case L1Token::Type::Float: 
		info.callerObject = std::make_shared<FloatObject>(1.0f);
		return;
	case L1Token::Type::ArrayClosed: 
		info.callerObject = std::make_shared<ArrayObject>();
		break;
#pragma endregion 

	case L1Token::Type::BracketClosed:
		// this is too complex for now... take default ScriptObject functions
		info.callerObject = NullObject::get();
		return;

	case L1Token::Type::Identifier:
		// the script engine has to identify the caller object
		info.callerToken = *it;
		return;

	// this makes no sense:
	case L1Token::Type::Undefined:
	case L1Token::Type::Assign: 
	case L1Token::Type::Separator: 
	case L1Token::Type::BracketOpen:
	case L1Token::Type::ArrayOpen:
	case L1Token::Type::Dot:
	case L1Token::Type::Plus:
	case L1Token::Type::PlusAssign:
	case L1Token::Type::Minus:
	case L1Token::Type::MinusAssign: 
	case L1Token::Type::Multiply: 
	case L1Token::Type::MultiplyAssign: 
	case L1Token::Type::Divide: 
	case L1Token::Type::DivideAssign: 
	case L1Token::Type::Function: 
	case L1Token::Type::IdentifierAssign: 
	case L1Token::Type::StaticFunction:
	case L1Token::Type::OpenString:
		return;

	default:;
	}
}
