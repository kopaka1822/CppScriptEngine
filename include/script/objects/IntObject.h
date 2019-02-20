#pragma once
#include "NumericalObject.h"

namespace script
{
	class IntObject final : public NumericalObject<int>
	{
	public:
		explicit IntObject(int value);
		~IntObject() override final = default;

		static FunctionT getCtor();

		std::string toString() const override;
		ScriptObjectPtr clone() const final override;

		void divide(const int& other) override;
	};
}
