#include "../../include/script/Tokenizer.h"
#include <regex>
#include <array>
#include "script/tokens/L1Rule.h"

std::vector<script::L1Token> script::Tokenizer::getTokens(const std::string& command)
{
	std::vector<L1Token> tokens = getL1Tokens(command);
	finalizeL1Tokens(tokens);

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
		case '+':
			curToken = L1Token(L1Token::Type::Plus, position);
			break;
		case '-':
			curToken = L1Token(L1Token::Type::Minus, position);
			break;
		case '*':
			curToken = L1Token(L1Token::Type::Multiply, position);
			break;
		case '/':
			curToken = L1Token(L1Token::Type::Divide, position);
			break;
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

void script::Tokenizer::finalizeL1Tokens(std::vector<L1Token>& tokens)
{
	static const L1FloatRule1 floatRule1;
	static const L1FloatRule2 floatRule2;
	static const L1NumberRule numberRule;
	static const L1FunctionRule functionRule;
	static const std::array<const L1Rule*, 4> rules = {
		&floatRule1, &floatRule2,&numberRule, &functionRule
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
