#pragma once
#include <stdexcept>
#include <sstream>
#include "objects/ArrayObject.h"

namespace script
{
	class Exception : public std::runtime_error
	{
	public:
		explicit Exception(std::string message);
		Exception();
		Exception(const Exception& copy);
		~Exception() override = default;

		char const* what() const override;

	protected:
		std::stringstream stream;
	private:
		mutable std::string m_message;
	};

	class ObjectNotCloneableException final : public Exception
	{
	public:
		explicit ObjectNotCloneableException(const std::string& objectName);
	};

	class InvalidArgumentCount final : public Exception
	{
	public:
		explicit InvalidArgumentCount(const std::string& functionSignature, size_t expectedCount, const ArrayObjectPtr& args);
	};

	class InvalidArgumentType final : public Exception
	{
	public:
		explicit InvalidArgumentType(const std::string& functionSignature, size_t argIndex, const ScriptObject& invalidArg, const ArrayObject& args);
	};

	class InvalidFunctionName final : public Exception
	{
	public:
		explicit InvalidFunctionName(const std::string& functionSignature, const std::string& missingFunction);
	};

	class ParseError : public Exception
	{
	public:
		explicit ParseError(size_t position, const std::string& error = "");
		const size_t position;
	};

	class SyntaxError final : public ParseError
	{
	public:
		explicit SyntaxError(size_t position, const std::string& token, const std::string& error);
	};

	class IdentifierError final : public ParseError
	{
	public:
		explicit IdentifierError(size_t position, const std::string& identifier);
	};

	class BracketMismatch final : public ParseError
	{
	public:
		explicit BracketMismatch(size_t position, const std::string& expected, const std::string& found);
	};
}
