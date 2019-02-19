#pragma once
#include "NumericalObject.h"

namespace script
{
	class FloatObject final : public NumericalObject<float>
	{
	public:
		explicit FloatObject(float value);
		~FloatObject() override final = default;

		std::string toString() const override;
		ScriptObjectPtr clone() const final override;

		static ScriptObject::FunctionT getCtor();
	};
}
