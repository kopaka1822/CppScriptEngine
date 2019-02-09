#pragma once
#include "ValueObject.h"

namespace script
{
	class BoolObject final : public ValueObject<bool>
	{
	public:
		explicit BoolObject(bool value);
		std::string toString() const override;
		ScriptObjectPtr clone() const override;
	};
}
