#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include "../Script.h"

namespace script
{
	class ScriptObjectArray;

	class ScriptObject : public std::enable_shared_from_this<ScriptObject>
	{
	public:
		using FunctionT = std::function<ScriptObjectPtr(ScriptPtr<ScriptObjectArray>)>;

		ScriptObject(const ScriptObject&) = delete;
		ScriptObject(ScriptObject&&) = delete;
		ScriptObject& operator=(const ScriptObject&) = delete;
		ScriptObject& operator=(ScriptObject&&) = delete;

		virtual ~ScriptObject() = default;
		virtual std::string toString() const = 0;
		virtual ScriptObjectPtr clone() const = 0;

		ScriptObjectPtr invoke(const std::string& funcName, const ScriptPtr<ScriptObjectArray>& args);
		ScriptObjectPtr getMember(const std::string& memName);

	protected:
		ScriptObject();
		void addFunction(const std::string& name, const FunctionT& func);
		void addMember(const std::string& name, ScriptObjectPtr member);
	private:
		std::unordered_map<std::string, FunctionT> m_functions;
		std::unordered_map<std::string, ScriptObjectPtr> m_members;
	};
}
