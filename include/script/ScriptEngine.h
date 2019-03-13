#pragma once
#include <string>
#include <unordered_map>
#include "objects/ScriptObject.h"
#include "objects/ArrayObject.h"
#include "objects/IntObject.h"
#include "objects/StringObject.h"
#include "objects/NullObject.h"
#include "objects/BoolObject.h"
#include "objects/EnumObject.h"
#include "Util.h"
#include <unordered_set>

namespace script
{
	class ScriptEngine
	{
	public:
		enum InitFlags
		{
			Nothing = 0,
			// static function to construct primitive types
			PrimitiveConstructor = 1,
			// static console object to read from and write to the console
			ConsoleClass = 1 << 1,
			// static system object 
			SystemClass = 1 << 2,
			// static clock with date/time and timestamp classes
			ClockClass = 1 << 3,
			// static IO object to read/write to files
			IOClass = 1 << 4,
			// static Engine object to interact with this ScriptEngine instance
			EngineClass = 1 << 5,

			All = 0xFFFFFFFF
		};

		explicit ScriptEngine(InitFlags flags = All);

		/// \brief executes the given command and returns the result
		/// \param command command to execute
		ScriptObjectPtr execute(const std::string& command);

		/// \brief retrieves the object with the given name
		/// \param object name
		/// \return pointer to the object or nullptr if not found
		ScriptObjectPtr getObject(const std::string& object) const;

		/// \brief sets or removes the object with the given name
		/// \param name object name
		/// \param object valid object to add or nullptr if object should be removed
		void setObject(const std::string& name, const ScriptObjectPtr& object);

		/// \brief retrieves the object with the given name
		/// \param object name
		/// \return pointer to the object or nullptr if not found
		ScriptObjectPtr getStaticObject(const std::string& object) const;

		/// \brief sets the object with the given name
		/// \param name object name
		/// \param object valid object
		void setStaticObject(const std::string& name, const ScriptObjectPtr& object);

		/// \brief retrieves the function with the given name
		/// \return valid function or empty function if not found
		ScriptObject::FunctionT getStaticFunction(const std::string& name);

		/// \brief sets the function with the given name
		/// \param name function name
		/// \param function valid function
		void setStaticFunction(const std::string& name, const ScriptObject::FunctionT& function);

		const auto& getObjects() const
		{
			return m_objects;
		}

		const auto& getStaticObjects() const
		{
			return m_staticObjects;
		}

		const auto& getStaticFunctions() const
		{
			return m_staticFunctions;
		}

		/// \brief retrieves a list of all possible auto-completions regarding to the text
		std::vector<std::string> getAutocomplete(const std::string& text);
	private:
		static void addAllFunctions(std::unordered_set<std::string>& set, const ScriptObject& obj);

		std::unordered_map<std::string, ScriptObjectPtr> m_objects;
		std::unordered_map<std::string, ScriptObjectPtr> m_staticObjects;
		std::unordered_map<std::string, ScriptObject::FunctionT> m_staticFunctions;
	};
}
