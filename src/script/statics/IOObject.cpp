#include "../../../include/script/statics/IOObject.h"
#include <fstream>
#include <iostream>
#include <mutex>

script::IOObject::IOObject(ScriptEngine& engine)
	:
m_engine(engine)
{
	addFunction("readFile", Util::makeFunction(this, &IOObject::readFile, "string IOObject::readFile(string filename)"));
	addFunction("writeFile", Util::makeFunction(this, &IOObject::writeFile, "IOObject::writeFile(string filename, string text)"));
	addFunction("appendFile", Util::makeFunction(this, &IOObject::appendFile, "IOObject::appendFile(string filename, string text)"));
	addFunction("executeFile", Util::makeFunction(this, &IOObject::executeFile, "IOObject::executeFile(string filename)"));
	addFunction("getExecuteDebugOutput", Util::makeFunction(this, &IOObject::getExecuteDebugOutput, "bool IOObject::getExecuteDebugOutput()"));
	addFunction("setExecuteDebugOutput", Util::makeFunction(this, &IOObject::setExecuteDebugOutput, "IOObject::setExecuteDebugOutput(bool enable)"));
}

std::string script::IOObject::readFile(const std::string& filename) const
{
	std::ifstream file(filename.c_str());
	if (!file.is_open())
		throw std::runtime_error("could not open file " + filename);

	std::string res;

	// allocate enough space
	file.seekg(0, std::ios::end);
	res.reserve(size_t(file.tellg()));
	file.seekg(0, std::ios::beg);

	// read contents
	res.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	
	return res;
}

void script::IOObject::writeFile(const std::string& filename, const std::string& data) const
{
	writeOstream(filename, data, std::ios::out | std::ios::trunc);
}

void script::IOObject::appendFile(const std::string& filename, const std::string& data) const
{
	writeOstream(filename, data, std::ios::out | std::ios::app);
}

script::ScriptObjectPtr script::IOObject::executeFile(const std::string& filename)
{
	std::lock_guard<IntMutex> recCounter(m_recursionCounter);
	if (m_recursionCounter.getValue() > 50)
		throw std::runtime_error("recursion level too deep - aborted to prevent stack overflow");

	auto text = readFile(filename);

	// output information
	std::cout << "executing \"" << filename << "\":" << std::endl;

	// read lines and remember line number
	size_t lineNo = 1;
	size_t commandStartLine = 0;
	bool isComment = false;
	std::string command;
	command.reserve(128);
	ScriptObjectPtr lastObject = NullObject::get();

	for(auto c = text.begin(), end = text.end(); c != end; ++c)
	{
		// new line
		if (*c == '\n')
		{
			lineNo++;
			isComment = false;
		}

		// ignore values in comments
		if (isComment) continue;

		// start the command with the first non whitespace character
		if(command.empty() && isspace(*c))
			continue;

		// line number should be number of first character
		if (command.empty())
			commandStartLine = lineNo;

		switch (*c)
		{
		case '\\':
			// some escape character => ignore and skip character
			command += *c;
			if ((c + 1) != end)
			{
				++c;
				command += *c;
			}
			continue;
		case ';':
			// end of command
			try
			{
				if(m_executeDebugOutput)
					std::cout << "line " << commandStartLine << ": " << command << std::endl;

				lastObject = m_engine.execute(command);
			}
			catch (std::exception& e)
			{
				throw std::runtime_error(filename + " line " + std::to_string(commandStartLine) + std::string(": ") + e.what());
			}
			command.resize(0);
			continue;
		case '/':
			// comment start?
			if((c + 1) != end && *(c + 1) == '/')
			{
				isComment = true;
				continue;
			}
			break;
		}
		
		command += *c;
	}

	if (!command.empty())
		throw std::runtime_error(filename + " missing ; at the end of the file");

	return lastObject;
}

void script::IOObject::setExecuteDebugOutput(bool enable)
{
	m_executeDebugOutput = enable;
}

bool script::IOObject::getExecuteDebugOutput() const
{
	return m_executeDebugOutput;
}

void script::IOObject::writeOstream(const std::string& filename, const std::string& data,
	std::ios_base::openmode mode) const
{
	std::ofstream file(filename, mode);
	file << data;
}
