#pragma once
#include "L2Token.h"

namespace script
{
	class L2IdentifierAssignToken final : public L2Token
	{
	public:
		L2IdentifierAssignToken(std::string name, std::unique_ptr<L2Token> value);
		ScriptObjectPtr execute(ScriptEngine& engine) const override;

	private:
		std::string m_name;
		std::unique_ptr<L2Token> m_value;
	};
}