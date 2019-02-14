#pragma once
#include <vector>
#include "tokens/L1Token.h"
#include <memory>
#include "tokens/L2Token.h"

namespace script
{
	class Tokenizer
	{
	public:
		Tokenizer() = delete;

		std::vector<L1Token> getTokens(const std::string& command);

		static std::vector<L1Token> getL1Tokens(const std::string& command);
		static void applyL1Rules(std::vector<L1Token>& tokens);
		static void verifyBrackets(const std::vector<L1Token>& tokens);
		static std::vector<std::unique_ptr<L2Token>> getL2Tokens(const std::vector<L1Token>& tokens);
	};
}
