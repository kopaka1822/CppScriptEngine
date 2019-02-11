#pragma once
#include "ValueComparableObject.h"
#include "script/Util.h"

namespace script
{
	/// \brief adds basic numerical operations (add, subtract...)
	template<class T>
	class NumericalObject : public ValueComparableObject<T>
	{
	public:
		~NumericalObject() override = default;

		virtual void add(const T& other)
		{
			this->m_value += other;
		}

		virtual void subtract(const T& other)
		{
			this->m_value -= other;
		}

		virtual void divide(const T& other)
		{
			this->m_value /= other;
		}

		virtual void multiply(const T& other)
		{
			this->m_value *= other;
		}

		virtual void negate(const T& other)
		{
			this->m_value = -this->m_value;
		}
	protected:
		explicit NumericalObject(T value)
			:
			ValueComparableObject<T>(std::move(value))
		{
			setFunctions();
		}

		template<class... ArgT>
		explicit NumericalObject(ArgT&&... args)
			:
			ValueComparableObject<T>(std::forward<ArgT>(args)...)
		{
			setFunctions();
		}

		NumericalObject()
		{
			setFunctions();
		}

	private:
		void setFunctions()
		{
			this->addFunction("add", Util::makeFunction(this, &NumericalObject<T>::add, "NumericalObject<T>::add(T)"));
			this->addFunction("subtract", Util::makeFunction(this, &NumericalObject<T>::subtract, "NumericalObject<T>::subtract(T)"));
			this->addFunction("divide", Util::makeFunction(this, &NumericalObject<T>::divide, "NumericalObject<T>::divide(T)"));
			this->addFunction("multiply", Util::makeFunction(this, &NumericalObject<T>::multiply, "NumericalObject<T>::multiply(T)"));
			this->addFunction("negate", Util::makeFunction(this, &NumericalObject<T>::negate, "NumericalObject<T>::negate()"));
		}
	};
}
