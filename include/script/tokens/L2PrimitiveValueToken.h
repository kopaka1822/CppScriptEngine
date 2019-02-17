#pragma once
#include "L2Token.h"

namespace script
{
	template<class T>
	class L2PrimitiveValueToken final : public L2Token
	{
	public:
		explicit L2PrimitiveValueToken(T value)
			:
			m_value(value)
		{}
		ScriptObjectPtr execute(ScriptEngine&) const override
		{
			return Util::makeObject<T>(m_value);
		}
	private:
		T m_value;
	};
}