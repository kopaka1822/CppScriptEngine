#pragma once
#include "L2Token.h"

namespace script
{
	class L2PropertySetterToken final : public L2Token
	{
	public:
		L2PropertySetterToken(std::unique_ptr<L2Token> object, std::unique_ptr<L2Token> arg, std::string propName,
			size_t position);
		ScriptObjectPtr execute(ScriptEngine& engine) const override;

	private:
		std::unique_ptr<L2Token> m_object;
		std::unique_ptr<L2Token> m_arg;
		std::string m_funcName;
		size_t m_position;
	};
}