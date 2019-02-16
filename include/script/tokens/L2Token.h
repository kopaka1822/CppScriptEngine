#pragma once
#include "../ScriptEngine.h"

namespace script
{
	class L2Token
	{
	public:
		virtual ~L2Token() = default;
		virtual ScriptObjectPtr execute(ScriptEngine& engine) const = 0;
	};

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

	class L2IdentifierToken final : public L2Token
	{
	public:
		L2IdentifierToken(std::string name, size_t position)
			: m_name(std::move(name)),
			  m_position(position)
		{}

		ScriptObjectPtr execute(ScriptEngine& engine) const override
		{
			auto obj = engine.getObject(m_name);
			if (!obj)
				throw IdentifierError(m_position, m_name);

			return obj;
		}
	private:
		std::string m_name;
		size_t m_position;
	};

	class L2StaticIdentifierToken : public L2Token
	{
	public:
		L2StaticIdentifierToken(std::string name, size_t position)
			: m_name(std::move(name)),
			m_position(position)
		{}

		ScriptObjectPtr execute(ScriptEngine& engine) const override
		{
			auto obj = engine.getStaticObject(m_name);
			if (!obj)
				throw IdentifierError(m_position, m_name);

			return obj;
		}
	private:
		std::string m_name;
		size_t m_position;
	};

	class L2IdentifierAssignToken final : public L2Token
	{
	public:
		L2IdentifierAssignToken(std::string name, std::unique_ptr<L2Token> value)
			:
		m_name(move(name)),
		m_value(move(value))
		{}

		ScriptObjectPtr execute(ScriptEngine& engine) const override
		{
			auto obj = m_value->execute(engine);
			engine.setObject(m_name, obj);
			return obj;
		}
	private:
		std::string m_name;
		std::unique_ptr<L2Token> m_value;
	};

	class L2ArgumentListToken final : public L2Token
	{
	public:
		ScriptObjectPtr execute(ScriptEngine& engine) const override
		{
			// make array from arguments
			auto arr = std::make_shared<ArrayObject>();
			for (auto i = m_values.begin(), end = m_values.end(); i != end; ++i)
				arr->add((*i)->execute(engine));

			return arr;
		}

		void add(std::unique_ptr<L2Token> value)
		{
			m_values.emplace_back(move(value));
		}
	private:
		std::vector<std::unique_ptr<L2Token>> m_values;
	};

	class L2FunctionToken : public L2Token
	{
	public:
		L2FunctionToken(std::unique_ptr<L2Token> value, std::string name, size_t position, std::unique_ptr<L2ArgumentListToken> args)
			:
		m_value(move(value)),
		m_position(position),
		m_funcName(name),
		m_args(move(args))
		{}

		ScriptObjectPtr execute(ScriptEngine& engine) const override
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
	private:
		std::unique_ptr<L2Token> m_value;
		size_t m_position;
		std::string m_funcName;
		std::unique_ptr<L2ArgumentListToken> m_args;
	};

	class L2PropertyGetterToken final : public L2Token
	{
	public:
		L2PropertyGetterToken(std::unique_ptr<L2Token> object, std::string propName, size_t position)
			: m_object(move(object)),
			  m_funcName(propName),
			  m_position(position)
		{
		}

		ScriptObjectPtr execute(ScriptEngine& engine) const override
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

	private:
		std::unique_ptr<L2Token> m_object;
		std::string m_funcName;
		size_t m_position;
	};

	class L2PropertySetterToken final : public L2Token
	{
	public:
		L2PropertySetterToken(std::unique_ptr<L2Token> object, std::unique_ptr<L2Token> arg, std::string propName, size_t position)
			: m_object(move(object)),
			m_arg(move(arg)),
			m_funcName(propName),
			m_position(position)
		{

		}

		ScriptObjectPtr execute(ScriptEngine& engine) const override
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

	private:
		std::unique_ptr<L2Token> m_object;
		std::unique_ptr<L2Token> m_arg;
		std::string m_funcName;
		size_t m_position;
	};

	class L2OperatorToken : public L2Token
	{
	public:
		/// \brief token for unary and binary operators
		/// \param left left operand
		/// \param right right operand or null if unary operator
		/// \param position 
		/// \param funcName operator name (add)
		/// \param clone object will be cloned before operator invocation
		/// \param opSign operator sign (+)
		L2OperatorToken(std::unique_ptr<L2Token> left, std::unique_ptr<L2Token> right, size_t position, std::string funcName,
			bool clone, std::string opSign)
		:
		m_left(move(left)), 
		m_right(move(right)), 
		m_position(position), 
		m_clone(clone), 
		m_funcName(move(funcName)), 
		m_opSign(move(opSign))
		{}

		ScriptObjectPtr execute(ScriptEngine& engine) const override
		{
			ScriptObjectPtr right;
			if(m_right)
				right = m_right->execute(engine);
			
			auto left = m_left->execute(engine);

			// clone object
			if(m_clone)
			{
				try
				{
					left = left->clone();
				}
				catch (std::exception& e)
				{
					throw ParseError(m_position, "object must be cloneable to use the " + m_opSign + " operator: " + std::string(e.what()));
				}
			}

			// call the appropriate function
			if(m_right) // binary operator
			{
				
				try
				{
					left = left->invoke(m_funcName, Util::makeArray(right));
				}
				catch (const InvalidFunctionName&)
				{
					// add line number information
					throw ParseError(m_position, "object requires a " + m_funcName + "(object) function to use the binary \"" + m_opSign + "\" operator");
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
					throw ParseError(m_position, "object requires a " + m_funcName + "() function to use the unary \"" + m_opSign + "\" operator");
				}
				catch (const std::exception& e)
				{
					// add line number information
					throw ParseError(m_position, m_funcName + " exception: " + std::string(e.what()));
				}
			}

			return left;
		}

	private:
		std::unique_ptr<L2Token> m_left;
		std::unique_ptr<L2Token> m_right;
		size_t m_position;
		bool m_clone;
		std::string m_funcName;
		std::string m_opSign;
	};
}
