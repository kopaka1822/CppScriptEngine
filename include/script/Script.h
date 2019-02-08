#pragma once
#include <memory>

namespace script
{
	class ScriptObject;
	template<class T>
	using ScriptPtr = std::shared_ptr<T>;
	using ScriptObjectPtr = ScriptPtr<ScriptObject>;
}