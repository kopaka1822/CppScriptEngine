#pragma once
#include "ScriptObject.h"

namespace script
{
	/// \brief extends ScriptObject by a T& getValue() interface function
	template<class T>
	class GetValueObject : public ScriptObject
	{
	public:
		~GetValueObject() override = default;

		virtual T& getValue() = 0;
	};
}