#pragma once
#include "ValueComparableObject.h"

namespace script
{
	class BoolObject final : public ValueComparableObject<bool>
	{
	public:
		explicit BoolObject(bool value);

		static FunctionT getCtor();

		std::string toString() const override;
		ScriptObjectPtr clone() const override;

		/// \brief reverses the value
		void invert();
	};
}
