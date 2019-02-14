#pragma once
#include "../ScriptEngine.h"

namespace script
{
	class L2Token
	{
	public:
		virtual ~L2Token() = default;

		
	};

	class ValueToken : public L2Token
	{
	public:
		virtual ScriptObjectPtr execute(ScriptEngine& engine) const = 0;
	};

	template<class T>
	class PrimitiveValueToken final : public ValueToken
	{
	public:
		explicit PrimitiveValueToken(T value)
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

	class IdentifierToken final : public ValueToken
	{
	public:
		IdentifierToken(std::string name, size_t position)
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

	class IdentifierAssignToken final : public ValueToken
	{
	public:
		IdentifierAssignToken(std::string name, std::unique_ptr<ValueToken> value)
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
		std::unique_ptr<ValueToken> m_value;
	};

	class ArgumentListToken final : ValueToken
	{
	public:
		ScriptObjectPtr execute(ScriptEngine& engine) const override
		{
			// make array from arguments
			auto arr = std::make_shared<ArrayObject>();
			for (const auto& v : m_values)
				arr->add(v->execute(engine));

			return arr;
		}

		void add(std::unique_ptr<ValueToken> value)
		{
			m_values.emplace_back(move(value));
		}
	private:
		std::vector<std::unique_ptr<ValueToken>> m_values;
	};

	class FunctionToken : ValueToken
	{
	public:
		FunctionToken(std::unique_ptr<ValueToken> value, std::string name, size_t position, std::unique_ptr<ArgumentListToken> args)
			:
		m_value(move(value)),
		m_position(position),
		m_funcName(name),
		m_args(move(args))
		{}

		ScriptObjectPtr execute(ScriptEngine& engine) const override
		{
			auto args = std::reinterpret_pointer_cast<ArrayObject>(m_args->execute(engine));
			auto obj = m_value->execute(engine);
			ScriptObjectPtr res;

			// call function on value
			try
			{
				res = obj->invoke(m_funcName, args);
			}
			catch (const InvalidFunctionName& e)
			{
				// add line number information
				throw ParseError(m_position, e.what());
			}
			catch (const std::exception& e)
			{
				// add line number information
				throw ParseError(m_position, m_funcName + " invocation threw an exception: " + std::string(e.what()));
			}

			return res;
		}
	private:
		std::unique_ptr<ValueToken> m_value;
		size_t m_position;
		std::string m_funcName;
		std::unique_ptr<ArgumentListToken> m_args;
	};
}
