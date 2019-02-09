#pragma once
#include "ScriptObject.h"

namespace script
{
	template<class T>
	class GetValueObject : public ScriptObject
	{
	public:
		~GetValueObject() override = default;

		virtual T& getValue() = 0;
	};
}