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

		struct AutocompleteInfo
		{
			// name of the underlying object (if syntax is callerToken.objectToken)
			L1Token callerToken = L1Token(L1Token::Type::Undefined, 0, "");
			// instance of the callerToken (may be used for primitive types)
			ScriptObjectPtr callerObject;
			// object that is currently being typed
			L1Token objectToken = L1Token(L1Token::Type::Undefined, 0, "");;
			// auto complete should propose nothing
			bool noProposal = false;
		};

		Tokenizer() = delete;

		static std::unique_ptr<L2Token> getExecutable(const std::string& command);

		static AutocompleteInfo getAutocomplete(const std::string& command);

		static std::vector<L1Token> getL1Tokens(const std::string& command, bool throwExceptions = true);
		static void applyL1Rules(std::vector<L1Token>& tokens);
		static void verifyBrackets(const std::vector<L1Token>& tokens);
		static std::unique_ptr<L2Token> getL2Tokens(std::vector<L1Token>::const_iterator& start, std::vector<L1Token>::const_iterator end, bool isArgumentList, OpReturnMode mode);

	private:
		static std::unique_ptr<L2Token> parseArgumentList(std::vector<L1Token>::const_iterator& start, std::vector<L1Token>::const_iterator end, L1Token::Type endToken, const std::string& type);
		static void handleOperatorAssign(std::vector<L1Token>::const_iterator& start, std::vector<L1Token>::const_iterator end, 
			std::unique_ptr<L2Token>& curToken, bool isArgumentList, std::string funcName, std::string opName);
		static char getEscapedChar(size_t position, char value);

		static void fillAutocompleteCaller(const std::vector<L1Token>& tokens, AutocompleteInfo& info);
	};
}
