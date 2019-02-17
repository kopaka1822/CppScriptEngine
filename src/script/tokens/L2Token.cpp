#include "../../../include/script/tokens/L2Token.h"

script::L2IdentifierToken::L2IdentifierToken(std::string name, size_t position) 
	: 
	m_name(std::move(name)),
	m_position(position)
{
}

script::ScriptObjectPtr script::L2IdentifierToken::execute(ScriptEngine& engine) const
{
	auto obj = engine.getObject(m_name);
	if (!obj)
		throw IdentifierError(m_position, m_name);

	return obj;
}

script::L2StaticIdentifierToken::L2StaticIdentifierToken(std::string name, size_t position)
	: 
	m_name(std::move(name)),
    m_position(position)
{
}

script::ScriptObjectPtr script::L2StaticIdentifierToken::execute(ScriptEngine& engine) const
{
	auto obj = engine.getStaticObject(m_name);
	if (!obj)
		throw IdentifierError(m_position, m_name);

	return obj;
}

script::L2IdentifierAssignToken::L2IdentifierAssignToken(std::string name, std::unique_ptr<L2Token> value):
	m_name(move(name)),
	m_value(move(value))
{
}

script::ScriptObjectPtr script::L2IdentifierAssignToken::execute(ScriptEngine& engine) const
{
	auto obj = m_value->execute(engine);
	engine.setObject(m_name, obj);
	return obj;
}

script::ScriptObjectPtr script::L2ArgumentListToken::execute(ScriptEngine& engine) const
{
	// make array from arguments
	auto arr = std::make_shared<ArrayObject>();
	for (auto i = m_values.begin(), end = m_values.end(); i != end; ++i)
		arr->add((*i)->execute(engine));

	return arr;
}

void script::L2ArgumentListToken::add(std::unique_ptr<L2Token> value)
{
	m_values.emplace_back(move(value));
}

script::L2FunctionToken::L2FunctionToken(std::unique_ptr<L2Token> value, std::string name, size_t position,
                                         std::unique_ptr<L2ArgumentListToken> args):
	m_value(move(value)),
	m_position(position),
	m_funcName(move(name)),
	m_args(move(args))
{
}

script::ScriptObjectPtr script::L2FunctionToken::execute(ScriptEngine& engine) const
{
	auto args = std::dynamic_pointer_cast<ArrayObject>(m_args->execute(engine));
	auto obj = m_value->execute(engine);

	// call function on value
	try
	{
		obj = obj->invoke(m_funcName, args);
	}
	catch (const InvalidFunctionName& e)
	{
		// add line number information
		throw ParseError(m_position, e.what());
	}
	catch (const std::exception& e)
	{
		// add line number information
		throw ParseError(m_position, m_funcName + " exception: " + std::string(e.what()));
	}

	return obj;
}

script::L2PropertyGetterToken::L2PropertyGetterToken(std::unique_ptr<L2Token> object, std::string propName,
                                                     size_t position): 
	m_object(move(object)),
	m_funcName(move(propName)),
    m_position(position)
{
}

script::ScriptObjectPtr script::L2PropertyGetterToken::execute(ScriptEngine& engine) const
{
	auto obj = m_object->execute(engine);
	try
	{
		obj = obj->invoke("get" + m_funcName, Util::makeArray());
	}
	catch (const InvalidFunctionName&)
	{
		// add line number information
		throw ParseError(m_position, "property getter " + m_funcName + " not found");
	}
	catch (const std::exception& e)
	{
		// add line number information
		throw ParseError(m_position, "get" + m_funcName + " exception: " + std::string(e.what()));
	}
	return obj;
}

script::L2PropertySetterToken::L2PropertySetterToken(std::unique_ptr<L2Token> object, std::unique_ptr<L2Token> arg,
                                                     std::string propName, size_t position): 
	m_object(move(object)),
	m_arg(move(arg)),
	m_funcName(move(propName)),
	m_position(position)
{
}

script::ScriptObjectPtr script::L2PropertySetterToken::execute(ScriptEngine& engine) const
{
	auto arg = m_object->execute(engine);
	auto obj = m_object->execute(engine);
	try
	{
		obj = obj->invoke("set" + m_funcName, Util::makeArray(arg));
	}
	catch (const InvalidFunctionName&)
	{
		// add line number information
		throw ParseError(m_position, "property setter " + m_funcName + " not found");
	}
	catch (const std::exception& e)
	{
		// add line number information
		throw ParseError(m_position, "set" + m_funcName + " exception: " + std::string(e.what()));
	}
	return obj;
}

script::L2OperatorToken::L2OperatorToken(std::unique_ptr<L2Token> left, std::unique_ptr<L2Token> right, size_t position,
                                         std::string funcName, bool clone, std::string opSign):
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
