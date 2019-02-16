#pragma once
#include <string>
#include <unordered_map>
#include "objects/ScriptObject.h"
#include "objects/ArrayObject.h"
#include "objects/IntObject.h"
#include "objects/StringObject.h"
#include "objects/NullObject.h"
#include "objects/BoolObject.h"
#include "Util.h"

namespace script
{
	class ScriptEngine
	{
	public:
		void execute(const std::string& command);

		/// \brief retrieves the object with the given name
		/// \param object name
		/// \return pointer to the object or nullptr if not found
		ScriptObjectPtr getObject(const std::string& object) const;

		/// \brief sets or removes the object with the given name
		/// \param name object name
		/// \param object valid object to add or nullptr if object should be removed
		void setObject(const std::string& name, ScriptObjectPtr object);

		/// \brief retrieves the object with the given name
		/// \param object name
		/// \return pointer to the object or nullptr if not found
		ScriptObjectPtr getStaticObject(const std::string& object) const;

		/// \brief sets the object with the given name
		/// \param name object name
		/// \param object valid object
		void setStaticObject(const std::string& name, ScriptObjectPtr object);
	private:
		std::unordered_map<std::string, ScriptObjectPtr> m_objects;
		std::unordered_map<std::string, ScriptObjectPtr> m_staticObjects;
	};
}