#pragma once
#include "L2Token.h"

namespace script
{
	class L2StaticFunctionToken final : public L2Token
	{
	public:
		L2StaticFunctionToken(std::string name, size_t position, std::unique_ptr<L2Token> args);
		ScriptObjectPtr execute(ScriptEngine& engine) const override;
	private:
		size_t m_position;
		std::string m_funcName;
		std::unique_ptr<L2Token> m_args;
	};
}
