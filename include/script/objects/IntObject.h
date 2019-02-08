#pragma once
#include "ValueObject.h"

namespace script
{
	class IntObject : public ValueObject<int>
	{
	public:
		explicit IntObject(int value);
		~IntObject() override final = default;

		std::string toString() const override;
		ScriptObjectPtr clone() const final override;

		void add(int i);
		void subtract(int i);
		void divide(int i);
		void multiply(int i);
	};
}
