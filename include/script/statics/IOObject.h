#pragma once
#include "../ScriptEngine.h"
#include "script/IntMutex.h"

namespace script
{
	class IOObject : public ScriptObject
	{
	public:
		IOObject(ScriptEngine& engine);
		~IOObject() override = default;

		/// \brief tries to read the entire file as a string
		/// \param filename 
		/// \return file contents
		std::string readFile(const std::string& filename) const;
		
		/// \brief writes data into the file (creates the file if it did not exits)
		/// \param filename 
		/// \param data 
		void writeFile(const std::string& filename, const std::string& data) const;

		/// \brief appends data to the file (creates the file if it did not exits)
		/// \param filename 
		/// \param data 
		void appendFile(const std::string& filename, const std::string& data) const;

		/// \brief opens the file and executes line by line with the script engine
		/// \param filename 
		/// \return the result of the last line
		ScriptObjectPtr executeFile(const std::string& filename);

		/// \brief indicates if executeFile() should output the command before executing it
		void setExecuteDebugOutput(bool enable);
		
		/// \brief indicates if executeFile() should output the command before executing it
		bool getExecuteDebugOutput() const;
	private:
		void writeOstream(const std::string& filename, const std::string& data, std::ios_base::openmode mode) const;

		ScriptEngine& m_engine;
		bool m_executeDebugOutput = false;
		IntMutex m_recursionCounter;
	};
}
