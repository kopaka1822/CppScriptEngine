#pragma once
#include "ValueObject.h"

namespace script
{
	
	/// \brief replaces the reference equals with a value equals (from ValueObject<T>::m_value)
	template<class T>
	class ValueComparableObject : public ValueObject<T>
	{
	public:
		~ValueComparableObject() override = default;

		bool equals(const ScriptObjectPtr& other) const override final
		{
			// same type?
			auto vco = dynamic_cast<ValueComparableObject<T>*>(other.get());
			if (vco == nullptr) return false;
			// only compare value
			return this->m_value == vco->m_value;
		}
	protected:
		explicit ValueComparableObject(T value)
			:
		ValueObject<T>(std::move(value))
		{}

		template<class... ArgT>
		explicit ValueComparableObject(ArgT&&... args)
			:
		ValueObject<T>(std::forward<ArgT>(args)...)
		{}

		ValueComparableObject() = default;
	};
}
