#include "../../include/script/Exception.h"
#include <numeric>

script::Exception::Exception(const std::string& message)
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

script::InvalidArgumentCount::InvalidArgumentCount(const std::string& functionSignature, size_t expectedCount, size_t actualCount)
	:
	InvalidArgumentCount(std::set<std::string>{functionSignature}, std::set<size_t>{ expectedCount }, actualCount)
{
	
}

script::InvalidArgumentCount::InvalidArgumentCount(std::set<std::string> functionSignatures,
	std::set<size_t> expectedCounts, size_t actualCount)
	:
expectedCounts(move(expectedCounts)),
functionSignatures(move(functionSignatures))
{
	stream << "invalid argument count for ";
	stream << std::accumulate(this->functionSignatures.begin(), this->functionSignatures.end(), std::string(""), [](const std::string& val1, const std::string& val2)
	{
		if (val1.empty()) return val2;
		return val1 + ", " + val2;
	});
	stream << ". expected ";
	stream << std::accumulate(this->expectedCounts.begin(), this->expectedCounts.end(), std::string(""), [](const std::string& val1, size_t val2)
	{
		if (val1.empty()) return std::to_string(val2);
		return val1 + ", " + std::to_string(val2);
	});
	stream << " but got " << actualCount;
}

script::InvalidArgumentType::InvalidArgumentType(const std::string& functionSignature, size_t argIndex,
	const ScriptObject& invalidArg, const std::string& expectedType)
{
	stream << functionSignature << " cannot convert argument " << (argIndex + 1) 
	<< " from " << invalidArg.type() << " to " << expectedType;
}

script::InvalidArgumentType::InvalidArgumentType(const std::string& message)
{
	stream << message;
}

script::InvalidArgumentConversion::InvalidArgumentConversion(std::string desiredType)
	:
desiredType(move(desiredType))
{
	stream << "could not convert to " << desiredType;
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

