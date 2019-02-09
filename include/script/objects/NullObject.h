#pragma once
#include "ScriptObject.h"

namespace script
{
	/// \brief represents the null constant. This class is a singleton
	class NullObject final : public ScriptObject
	{
		NullObject() = default;
	public:
		std::string toString() const override final;
		ScriptObjectPtr clone() const override final;

		static ScriptObjectPtr get();
	};
}