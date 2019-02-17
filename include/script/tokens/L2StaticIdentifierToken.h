#pragma once
#include "L2Token.h"

namespace script
{
	class L2StaticIdentifierToken final : public L2Token
	{
	public:
		L2StaticIdentifierToken(std::string name, size_t position);
		ScriptObjectPtr execute(ScriptEngine& engine) const override;

	private:
		std::string m_name;
		size_t m_position;
	};
}