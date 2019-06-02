#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include "../Script.h"

namespace script
{
	class ArrayObject;
	class BoolObject;

	class ScriptObject : public std::enable_shared_from_this<ScriptObject>
	{
	public:
		using FunctionT = std::function<ScriptObjectPtr(const ScriptPtr<ArrayObject>&)>;

		ScriptObject(const ScriptObject&) = delete;
		ScriptObject(ScriptObject&&) = delete;
		ScriptObject& operator=(const ScriptObject&) = delete;
		ScriptObject& operator=(ScriptObject&&) = delete;

		virtual ~ScriptObject() = default;
		virtual std::string toString() const;
		virtual ScriptObjectPtr clone() const;
		virtual bool equals(const ScriptObjectPtr& other) const;
		std::string type() const;
		size_t hashCode() const;

		ScriptObjectPtr invoke(const std::string& funcName, const ScriptPtr<ArrayObject>& args);
		/// \brief returns all registered functions (including getter and setter)
		std::vector<std::string> getFunctions() const;
		/// \brief returns all getter without the get prefix
		std::vector<std::string> getGetter() const;
		/// \brief returns all setter without the set prefix
		std::vector<std::string> getSetter() const;
	protected:
		ScriptObject();
		/// \brief adds a function to the internal list and throws an error if the name is ill formatted
		void addFunction(const std::string& name, const FunctionT& func);

		/// \brief adds a function that starts with a get prefix and returns the variable
		template<class T>
		void addGetter(const std::string& name, const T& variable);
		/// \brief adds a function that starts with a set prefix and sets the variable
		template<class T>
		void addSetter(const std::string& name, T& variable);
		/// \brief performs addGetter and addSetter
		template<class T>
		void addGetterSetter(const std::string& name, T& variable);
		/// \brief removes the function if it exists
		void removeFunction(const std::string& name) noexcept;
	private:
		std::unordered_map<std::string, FunctionT> m_functions;
	};

	template <class T>
	void ScriptObject::addGetterSetter(const std::string& name, T& variable)
	{
		addGetter(name, variable);
		addSetter(name, variable);
	}
}
