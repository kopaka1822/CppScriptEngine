#pragma once
#include "objects/ScriptObject.h"
#include "objects/ScriptObjectArray.h"
#include "objects/ValueObject.h"
#include "Exception.h"
#include <sstream>
#include "objects/NullObject.h"

namespace script
{
	class Util final
	{
	public:
		Util() = delete;

		/// \brief converts a class memberfunction into a ScriptObject compatible function
		/// \tparam TClass 
		/// \tparam TArgs 
		/// \param parent pointer of the class that owns this function
		/// \param thisPtr pointer for the member function
		/// \param func member function pointer
		/// \param functionSignature syntax: functionName(type1 name, type2...)
		/// \return ScriptObject compatible function
		template<class TClass, class... TArgs>
		static std::function<ScriptObjectPtr(ScriptObjectArrayPtr)> makeFunction(ScriptObject* parent, TClass* thisPtr, void(TClass::* func)(TArgs...), std::string functionSignature)
		{
			std::function<ScriptObjectPtr(ScriptObjectArrayPtr)> res = (
				[thisPtr, func, functionSignature, parent]
			(ScriptObjectArrayPtr args) -> ScriptObjectPtr
			{
				size_t argCount = std::tuple_size<std::tuple<TArgs...>>::value;
				if (argCount != args->count())
					throw InvalidArgumentCount(functionSignature, argCount, args);

				std::invoke(func, thisPtr, unpackArg<TArgs>(args, argCount, functionSignature)...);
				return parent->shared_from_this();
			});
			return res;
		}

		/// \brief converts a class memberfunction into a ScriptObject compatible function
		/// \tparam TClass 
		/// \tparam TArgs 
		/// \param thisPtr class pointer
		/// \param func member function pointer
		/// \param functionSignature syntax: functionName(type1 name, type2...)
		/// \return ScriptObject compatible function
		template<class TClass, class... TArgs>
		static std::function<ScriptObjectPtr(ScriptObjectArrayPtr)> makeFunction(TClass* thisPtr, void(TClass::* func)(TArgs...), std::string functionSignature)
		{
			return makeFunction(static_cast<ScriptObject*>(thisPtr), thisPtr, func, functionSignature);
		}

		/// \brief converts a class memberfunction into a ScriptObject compatible function
		/// \tparam TClass 
		/// \tparam TArgs 
		/// \param thisPtr class pointer
		/// \param func member function pointer
		/// \param functionSignature syntax: returnType functionName(type1 name, type2...)
		/// \return ScriptObject compatible function
		template<class TClass, class TReturn, class... TArgs>
		static std::function<ScriptObjectPtr(ScriptObjectArrayPtr)> makeReturnFunction(TClass* thisPtr, TReturn(TClass::* func)(TArgs...), std::string functionSignature)
		{
			static_assert(std::is_convertible<TReturn, ScriptObjectPtr>::value, "return type must be convertible to ScriptObjectPtr");

			std::function<ScriptObjectPtr(ScriptObjectArrayPtr)> res = (
				[thisPtr, func, functionSignature]
			(ScriptObjectArrayPtr args) -> ScriptObjectPtr
			{
				size_t argCount = std::tuple_size<std::tuple<TArgs...>>::value;
				if (argCount != args->count())
					throw InvalidArgumentCount(functionSignature, argCount, args);

				return std::invoke(func, thisPtr, unpackArg<TArgs>(args, argCount, functionSignature)...);
			});
			return res;
		}

		/// \brief converts a const class memberfunction into a ScriptObject compatible function
		/// \tparam TClass 
		/// \tparam TArgs 
		/// \param thisPtr class pointer
		/// \param func member function pointer
		/// \param functionSignature syntax: returnType functionName(type1 name, type2...)
		/// \return ScriptObject compatible function
		template<class TClass, class TReturn, class... TArgs>
		static std::function<ScriptObjectPtr(ScriptObjectArrayPtr)> makeReturnFunction(const TClass* thisPtr, TReturn(TClass::* func)(TArgs...) const, std::string functionSignature)
		{
			// its okay because this and function were const => no change will happen
			return makeReturnFunction(const_cast<TClass*>(thisPtr), reinterpret_cast<TReturn(TClass::*)(TArgs...)>(func), functionSignature);
		}

		template<class... TClass>
		static ScriptObjectArrayPtr makeArray(const TClass&... objects)
		{
			auto arr = std::make_shared<ScriptObjectArray>();
			makeArray(*arr, objects...);
			return arr;
		}

		template<class T>
		static ScriptPtr<GetValueObject<T>> toScriptObject(const T& value);

	private:
		template<class T>
		static T& unpackArg(ScriptObjectArrayPtr& args, size_t& argCount, const std::string& functionSignature, 
			std::enable_if_t<!std::is_pointer<T>::value, int> = 0)
		{
			const auto index = --argCount;
			const auto& objectPtr = args->get(index);

			return getGetValueObjectValue<T>(objectPtr.get(), args, index, functionSignature);
		}

		template<class T>
		static T unpackArg(ScriptObjectArrayPtr& args, size_t& argCount, const std::string& functionSignature, 
			std::enable_if_t<std::is_pointer<T>::value, int> = 0)
		{
			const auto index = --argCount;
			const auto& objectPtr = args->get(index);

			// is nullptr?
			if (objectPtr->equals(NullObject::get())) return nullptr;

			// remove pointer
			using value_type = std::remove_pointer_t<T>;

			return &(getGetValueObjectValue<value_type>(objectPtr.get(), args, index, functionSignature));
		}

		template<class T>
		static T& getGetValueObjectValue(ScriptObject* object, ScriptObjectArrayPtr& args, size_t index, const std::string& functionSignature)
		{
			using bare_type =
				std::remove_const_t< // const T => T
				std::remove_reference_t<T>>;

			// cast to value object
			// and transform const T& to T
			auto* valuePtr = dynamic_cast<GetValueObject<bare_type>*> // T& => T
				(object);

			if (valuePtr == nullptr)
				throw InvalidArgumentType(functionSignature, index, *object, args);

			return valuePtr->getValue();
		}

		template<class TFirst, class... TRest>
		static void makeArray(ScriptObjectArray& array, const TFirst& first, const TRest&... objects)
		{
			array.add(first);
			makeArray(array, objects...);
		}

		// end recursion
		static void makeArray(ScriptObjectArray&) {}
	};
}
