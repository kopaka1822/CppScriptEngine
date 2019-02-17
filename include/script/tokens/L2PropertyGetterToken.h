#pragma once
#include "L2Token.h"

namespace script
{
	class L2PropertyGetterToken final : public L2Token
	{
	public:
		L2PropertyGetterToken(std::unique_ptr<L2Token> object, std::string propName, size_t position);
		ScriptObjectPtr execute(ScriptEngine& engine) const override;

	private:
		std::unique_ptr<L2Token> m_object;
		std::string m_funcName;
		size_t m_position;
	};
}