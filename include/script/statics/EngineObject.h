#pragma once
#include "../ScriptEngine.h"

namespace script
{
	class EngineObject : public GetValueObject<ScriptEngine>
	{
	public:
		EngineObject(ScriptEngine& engine);
		~EngineObject() override = default;

		ScriptEngine& getValue() override;

		/// \brief executes the command and returns the result
		ScriptObjectPtr execute(const std::string& command);
		ScriptObjectPtr getObject(const std::string& name) const;
		ScriptObjectPtr getStaticObject(const std::string& name) const;
		void setObject(const std::string& name, ScriptObjectPtr object);

		std::vector<std::string> getObjects() const;
		std::vector<std::string> getStaticObjects() const;
		std::vector<std::string> getStaticFunctions() const;

	private:
		ScriptEngine& m_engine;
	};
}