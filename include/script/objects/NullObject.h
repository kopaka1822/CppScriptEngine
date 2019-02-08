#pragma once
#include "ScriptObject.h"

namespace script
{
	class NullObject final : public ScriptObject
	{
		NullObject() = default;
	public:
		std::string toString() const override final;
		ScriptObjectPtr clone() const override final;

		static ScriptObjectPtr get();
	};
}