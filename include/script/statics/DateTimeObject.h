#pragma once
#include <ctime>
#include "../objects/ScriptObject.h"

namespace script
{
	class DateTimeObject : public ScriptObject
	{
	public:
		explicit DateTimeObject(std::tm value);
		//DateTimeObject();
		~DateTimeObject() override = default;
		std::string toString() const override;
		ScriptObjectPtr clone() const override;
		bool equals(const ScriptObjectPtr& other) const override;

		int getSecond() const;
		int getMinute() const;
		int getHour() const;
		int getDay() const;
		int getMonth() const;
		int getYear() const;

	private:
		std::tm m_value;
	};
}