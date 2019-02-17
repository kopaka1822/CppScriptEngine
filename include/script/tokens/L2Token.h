#pragma once
#include <string>
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
		L2IdentifierToken(std::string name, size_t position);
		ScriptObjectPtr execute(ScriptEngine& engine) const override;

	private:
		std::string m_name;
		size_t m_position;
	};

	class L2StaticIdentifierToken final : public L2Token
	{
	public:
		L2StaticIdentifierToken(std::string name, size_t position);
		ScriptObjectPtr execute(ScriptEngine& engine) const override;

	private:
		std::string m_name;
		size_t m_position;
	};

	class L2IdentifierAssignToken final : public L2Token
	{
	public:
		L2IdentifierAssignToken(std::string name, std::unique_ptr<L2Token> value);
		ScriptObjectPtr execute(ScriptEngine& engine) const override;

	private:
		std::string m_name;
		std::unique_ptr<L2Token> m_value;
	};

	class L2ArgumentListToken final : public L2Token
	{
	public:
		ScriptObjectPtr execute(ScriptEngine& engine) const override;
		void add(std::unique_ptr<L2Token> value);

	private:
		std::vector<std::unique_ptr<L2Token>> m_values;
	};

	class L2FunctionToken final : public L2Token
	{
	public:
		L2FunctionToken(std::unique_ptr<L2Token> value, std::string name, size_t position,
		                std::unique_ptr<L2ArgumentListToken> args);
		ScriptObjectPtr execute(ScriptEngine& engine) const override;
	private:
		std::unique_ptr<L2Token> m_value;
		size_t m_position;
		std::string m_funcName;
		std::unique_ptr<L2ArgumentListToken> m_args;
	};

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
