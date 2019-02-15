#pragma once
#include "ValueComparableObject.h"

namespace script
{
	class StringObject final : public ValueComparableObject<std::string>
	{
	public:
		explicit StringObject(std::string value);
		~StringObject() override final = default;

		std::string toString() const override final;
		ScriptObjectPtr clone() const final override;

		void add(const std::string& value);
		int getLength() const;
	};
}
