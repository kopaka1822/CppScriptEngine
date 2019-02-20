#pragma once
#include "ValueComparableObject.h"
#include "ArrayObject.h"

namespace script
{
	class StringObject final : public ValueComparableObject<std::string>
	{
	public:
		explicit StringObject(std::string value);
		~StringObject() override final = default;

		static FunctionT getCtor();

		std::string toString() const override final;
		ScriptObjectPtr clone() const final override;

		ScriptObjectPtr add(const ArrayObjectPtr& args);
		int getLength() const;
	};
}
