#pragma once
#include "GetValueObject.h"

namespace script
{
	/// \brief hold a value of T as member (m_value) and implements GetValueObject<T>::getValue() by returning it
	template<class T>
	class ValueObject : public GetValueObject<T>
	{
	public:
		~ValueObject() override = default;

		T& getValue() override final
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