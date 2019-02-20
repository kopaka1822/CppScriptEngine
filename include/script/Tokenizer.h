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
		enum class OpReturnMode
		{
			End,
			PlusMinus,
			MultDiv
		};

		Tokenizer() = delete;

		static std::unique_ptr<L2Token> getExecutable(const std::string& command);

		static std::vector<L1Token> getL1Tokens(const std::string& command);
		static void applyL1Rules(std::vector<L1Token>& tokens);
		static void verifyBrackets(const std::vector<L1Token>& tokens);
		static std::unique_ptr<L2Token> getL2Tokens(std::vector<L1Token>::const_iterator& start, std::vector<L1Token>::const_iterator end, bool isArgumentList, OpReturnMode mode);

	private:
		static std::unique_ptr<L2Token> parseArgumentList(std::vector<L1Token>::const_iterator& start, std::vector<L1Token>::const_iterator end, L1Token::Type endToken, const std::string& type);
		static void handleOperatorAssign(std::vector<L1Token>::const_iterator& start, std::vector<L1Token>::const_iterator end, 
			std::unique_ptr<L2Token>& curToken, bool isArgumentList, std::string funcName, std::string opName);
		static char getEscapedChar(size_t position, char value);
	};
}
