#include "../../include/script/Exception.h"

script::Exception::Exception(std::string message)
	:
std::runtime_error("")
{
	stream << message;
}

script::Exception::Exception()
	:
std::runtime_error("")
{}

script::Exception::Exception(const Exception& copy)
	:
std::runtime_error(""),
stream(copy.stream.str())
{

}

char const* script::Exception::what() const
{
	m_message = stream.str();
	return m_message.c_str();
}

script::ObjectNotCloneableException::ObjectNotCloneableException(const std::string& objectName)
	:
Exception(objectName + " cannot be cloned")
{}

script::InvalidArgumentCount::InvalidArgumentCount(const std::string& functionSignature, size_t expectedCount,
	const ArrayObjectPtr& args)
{
	stream << "invalid argument count for " << functionSignature
		<< ". expected " << expectedCount << " argument" << (expectedCount == 1?"":"s")
		<< " but got " << args->count()
		<< ". args: " + args->toString();
}

script::InvalidArgumentType::InvalidArgumentType(const std::string& functionSignature, size_t argIndex,
	const ScriptObject& invalidArg, const ArrayObject& args)
{
	stream << "invalid argument type for " << functionSignature << ". argument " << argIndex
		<< " ('" << invalidArg.toString() << "' of type '" << invalidArg.type() << "') is invalid"
		<< ". args: " << args.toString();
}

script::InvalidFunctionName::InvalidFunctionName(const std::string& functionSignature,
	const std::string& missingFunction)
{
	stream << functionSignature << " function \"" << missingFunction << "\" not found";
}

script::ParseError::ParseError(size_t position, const std::string& error)
	:
position(position)
{
	stream << error;
}


script::SyntaxError::SyntaxError(size_t position, const std::string& token, const std::string& error)
	:
ParseError(position)
{
	stream << "syntax error: invalid token \"" << token << "\". " << error;
}

script::IdentifierError::IdentifierError(size_t position, const std::string& identifier)
	: 
ParseError(position)
{
	stream << "identifier \"" << identifier << "\" not found";
}

script::BracketMismatch::BracketMismatch(size_t position, const std::string& expected, const std::string& found)
	:
ParseError(position)
{
	stream << "bracket mismatch: expected " << expected << " but got " << found;
}

