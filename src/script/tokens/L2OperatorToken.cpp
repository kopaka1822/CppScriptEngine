#include "../../../include/script/tokens/L2OperatorToken.h"

script::L2OperatorToken::L2OperatorToken(std::unique_ptr<L2Token> left, std::unique_ptr<L2Token> right, size_t position,
	std::string funcName, bool clone, std::string opSign) :
	m_left(move(left)),
	m_right(move(right)),
	m_position(position),
	m_clone(clone),
	m_funcName(move(funcName)),
	m_opSign(move(opSign))
{
}

script::ScriptObjectPtr script::L2OperatorToken::execute(ScriptEngine& engine) const
{
	ScriptObjectPtr right;
	if (m_right)
		right = m_right->execute(engine);

	auto left = m_left->execute(engine);

	// clone object
	if (m_clone)
	{
		try
		{
			left = left->clone();
		}
		catch (std::exception& e)
		{
			throw ParseError(
				m_position, "object must be cloneable to use the " + m_opSign + " operator: " + std::string(e.what()));
		}
	}

	// call the appropriate function
	if (m_right) // binary operator
	{
		try
		{
			left = left->invoke(m_funcName, Util::makeArray(right));
		}
		catch (const InvalidFunctionName&)
		{
			// add line number information
			throw ParseError(
				m_position,
				"object requires a " + m_funcName + "(object) function to use the binary \"" + m_opSign +
				"\" operator");
		}
		catch (const std::exception& e)
		{
			// add line number information
			throw ParseError(m_position, m_funcName + " exception: " + std::string(e.what()));
		}
	}
	else // unary operator
	{
		try
		{
			left = left->invoke(m_funcName, Util::makeArray());
		}
		catch (const InvalidFunctionName&)
		{
			// add line number information
			throw ParseError(
				m_position,
				"object requires a " + m_funcName + "() function to use the unary \"" + m_opSign + "\" operator");
		}
		catch (const std::exception& e)
		{
			// add line number information
			throw ParseError(m_position, m_funcName + " exception: " + std::string(e.what()));
		}
	}

	return left;
}
