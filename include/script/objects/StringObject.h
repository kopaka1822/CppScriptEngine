#pragma once
#include "ValueObject.h"

namespace script
{
	class StringObject final : public ValueObject<std::string>
	{
	public:
		explicit StringObject(std::string value);
		~StringObject() override final = default;

		std::string toString() const override final;
		ScriptObjectPtr clone() const final override;

		void add(const std::string& value);
		size_t length() const;
	};
}