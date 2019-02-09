#pragma once
#include <stdexcept>
#include <sstream>
#include "objects/ScriptObjectArray.h"

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

	class ObjectNotCloneableException : public Exception
	{
	public:
		explicit ObjectNotCloneableException(const std::string& objectName);
	};

	class InvalidArgumentCount : public Exception
	{
	public:
		explicit InvalidArgumentCount(const std::string& functionSignature, int expectedCount, const ScriptObjectArrayPtr& args);
	};

	class InvalidArgumentType : public Exception
	{
	public:
		explicit InvalidArgumentType(const std::string& functionSignature, int argIndex, const ScriptObject& invalidArg, const ScriptObjectArrayPtr& args);
	};
}
