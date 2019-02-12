#pragma once
#include <vector>
#include "tokens/L1Token.h"
#include <memory>

namespace script
{
	class Tokenizer
	{
	public:
		Tokenizer() = delete;

		std::vector<L1Token> getTokens(const std::string& command);

		static std::vector<L1Token> getL1Tokens(const std::string& command);
		static void finalizeL1Tokens(std::vector<L1Token>& tokens);
	};
}
