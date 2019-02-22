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
		std::vector<std::string> getFunctions() const;
	protected:
		ScriptObject();
		void addFunction(const std::string& name, const FunctionT& func);

	private:
		std::unordered_map<std::string, FunctionT> m_functions;
	};
}
