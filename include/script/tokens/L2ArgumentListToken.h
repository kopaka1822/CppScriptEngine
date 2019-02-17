#pragma once
#include "L2Token.h"

namespace script
{
	class L2ArgumentListToken final : public L2Token
	{
	public:
		ScriptObjectPtr execute(ScriptEngine& engine) const override;
		void add(std::unique_ptr<L2Token> value);

	private:
		std::vector<std::unique_ptr<L2Token>> m_values;
	};
}