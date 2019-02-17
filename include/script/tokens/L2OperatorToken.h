#pragma once
#include "L2Token.h"

namespace script
{
	class L2OperatorToken final : public L2Token
	{
	public:
		/// \brief token for unary and binary operators
		/// \param left left operand
		/// \param right right operand or null if unary operator
		/// \param position 
		/// \param funcName operator name (add)
		/// \param clone object will be cloned before operator invocation
		/// \param opSign operator sign (+)
		L2OperatorToken(std::unique_ptr<L2Token> left, std::unique_ptr<L2Token> right, size_t position,
			std::string funcName,
			bool clone, std::string opSign);
		ScriptObjectPtr execute(ScriptEngine& engine) const override;

	private:
		std::unique_ptr<L2Token> m_left;
		std::unique_ptr<L2Token> m_right;
		size_t m_position;
		bool m_clone;
		std::string m_funcName;
		std::string m_opSign;
	};
}