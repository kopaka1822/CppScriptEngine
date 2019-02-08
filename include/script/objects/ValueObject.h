#pragma once
#include "ScriptObject.h"

namespace script
{
	template<class T>
	class ValueObject : public ScriptObject
	{
	public:
		~ValueObject() override = default;

		const T& getValue() const
		{
			return m_value;
		}
	protected:
		explicit ValueObject(T value)
			:
			m_value(std::move(value))
		{}
		template<class... ArgT>
		explicit ValueObject(ArgT&&... args)
			:
			m_value(std::forward<ArgT>(args)...)
		{}
		ValueObject() = default;

		T m_value;
	};
}