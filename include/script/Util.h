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
		/// \param functionSignature syntax: className::functionName(type1 name, type2...)
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
		/// \param functionSignature syntax: className::functionName(type1 name, type2...)
		/// \return ScriptObject compatible function
		template<class TClass, class... TArgs>
		static std::function<ScriptObjectPtr(ScriptObjectArrayPtr)> makeFunction(TClass* thisPtr, void(TClass::* func)(TArgs...), std::string functionSignature)
		{
			return makeFunction(static_cast<ScriptObject*>(thisPtr), thisPtr, func, functionSignature);
		}

		/*/// \brief converts a class memberfunction into a ScriptObject compatible function
		/// \tparam TClass 
		/// \tparam TArgs 
		/// \param thisPtr class pointer
		/// \param func member function pointer
		/// \param functionSignature syntax: className::functionName(type1 name, type2...)
		/// \return ScriptObject compatible function
		template<class TClass, class... TArgs>
		static std::function<ScriptObjectPtr(ScriptObjectArrayPtr)> makeFunction(const TClass* thisPtr, void(TClass::* func)(TArgs...) const, std::string functionSignature)
		{
			// its okay because this and function were const => no change will happen
			return makeFunction(const_cast<TClass*>(thisPtr), reinterpret_cast<void(TClass::*)(TArgs...)>(func), functionSignature);
		}*/

		

		/// \brief converts a class memberfunction into a ScriptObject compatible function
		/// \tparam TClass 
		/// \tparam TArgs 
		/// \param thisPtr class pointer
		/// \param func member function pointer
		/// \param functionSignature syntax: returnType className::functionName(type1 name, type2...)
		/// \return ScriptObject compatible function
		template<class TClass, class TReturn, class... TArgs>
		static std::function<ScriptObjectPtr(ScriptObjectArrayPtr)> makeReturnFunction(TClass* thisPtr, TReturn(TClass::* func)(TArgs...), std::string functionSignature)
		{
			std::function<ScriptObjectPtr(ScriptObjectArrayPtr)> res = (
				[thisPtr, func, functionSignature]
			(ScriptObjectArrayPtr args) -> ScriptObjectPtr
			{
				size_t argCount = std::tuple_size<std::tuple<TArgs...>>::value;
				if (argCount != args->count())
					throw InvalidArgumentCount(functionSignature, argCount, args);

				if constexpr(std::is_convertible<TReturn, ScriptObjectPtr>::value)
					// return type is already a script object
					return std::invoke(func, thisPtr, unpackArg<TArgs>(args, argCount, functionSignature)...);
				else
					// return type must be transformed to a script object
					return toScriptObject(std::invoke(func, thisPtr, unpackArg<TArgs>(args, argCount, functionSignature)...));
			});
			return res;
		}

		/// \brief converts a const class memberfunction into a ScriptObject compatible function
		/// \tparam TClass 
		/// \tparam TArgs 
		/// \param thisPtr class pointer
		/// \param func member function pointer
		/// \param functionSignature syntax: returnType className::functionName(type1 name, type2...)
		/// \return ScriptObject compatible function
		template<class TClass, class TReturn, class... TArgs>
		static std::function<ScriptObjectPtr(ScriptObjectArrayPtr)> makeReturnFunction(const TClass* thisPtr, TReturn(TClass::* func)(TArgs...) const, std::string functionSignature)
		{
			// its okay because this and function were const => no change will happen
			return makeReturnFunction(const_cast<TClass*>(thisPtr), reinterpret_cast<TReturn(TClass::*)(TArgs...)>(func), functionSignature);
		}

		/// \brief puts the arguments into a ScriptObjectArrayPtr
		template<class... TClass>
		static ScriptObjectArrayPtr makeArray(const TClass&... objects)
		{
			auto arr = std::make_shared<ScriptObjectArray>();
			makeArray(*arr, objects...);
			return arr;
		}

		/// \brief converts the T value into a GetValueObject<T>
		/// this is used to convert basic return types of functions to SciptObjects
		template<class T>
		static ScriptObjectPtr toScriptObject(const T& value);

		/// \brief calls toScriptObject with an std::string parameter
		static ScriptObjectPtr toScriptObject(const char* text);
	private:
		// helper functions to remove the shared_ptr wrapper
		template<class T> struct remove_shared { typedef T type; };
		template<class T> struct remove_shared<std::shared_ptr<T>> { typedef T type; };
		template<class T> struct remove_shared<std::shared_ptr<T>&> { typedef T type; };
		template<class T> struct remove_shared<const std::shared_ptr<T>> { typedef T type; };
		template<class T> struct remove_shared<const std::shared_ptr<T>&> { typedef T type; };
		template<class T> using remove_shared_t = typename  remove_shared<T>::type;

		/// \brief converts the argument at args->get(argCount - 1) to T where T is a shared_ptr<ScriptObject>
		template<class T>
		static std::remove_reference_t<T> unpackArg(ScriptObjectArrayPtr& args, size_t& argCount, const std::string& functionSignature,
			// convertible to ScriptObjectPtr
			std::enable_if_t<std::is_convertible<T, ScriptObjectPtr>::value, int> = 0)
		{
			const auto index = --argCount;
			const auto& objectPtr = args->get(index);

			// T is a base of ScriptObject
			// try to convert objectPtr to T
			using bare_type = remove_shared_t<T>;
			std::shared_ptr<bare_type> res = std::dynamic_pointer_cast<bare_type>(objectPtr);
			if (!res)
				throw InvalidArgumentType(functionSignature, index, *objectPtr, args);

			return res;
		}

		/// \brief converts the argument at args->get(argCount - 1) to T if the argument has the type GetValueObject<T>
		template<class T>
		static T& unpackArg(ScriptObjectArrayPtr& args, size_t& argCount, const std::string& functionSignature, 
			// not convertible to ScriptObjectPtr and not pointer
			std::enable_if_t<!std::is_convertible<T, ScriptObjectPtr>::value, int> = 0, std::enable_if_t<!std::is_pointer<T>::value, int> = 0)
		{
			const auto index = --argCount;
			const auto& objectPtr = args->get(index);

			return getGetValueObjectValue<T>(objectPtr.get(), args, index, functionSignature);
		}

		/// \brief converts the argument at args->get(argCount - 1) to T* if the argument has the type GetValueObject<T> or NullObject
		template<class T>
		static T unpackArg(ScriptObjectArrayPtr& args, size_t& argCount, const std::string& functionSignature, 
			// not convertible to ScriptObjectPtr but pointer
			std::enable_if_t<!std::is_convertible<T, ScriptObjectPtr>::value, int> = 0, std::enable_if_t<std::is_pointer<T>::value, int> = 0)
		{
			const auto index = --argCount;
			const auto& objectPtr = args->get(index);

			// is nullptr?
			if (objectPtr->equals(NullObject::get())) return nullptr;

			// remove pointer
			using value_type = std::remove_pointer_t<T>;

			return &(getGetValueObjectValue<value_type>(objectPtr.get(), args, index, functionSignature));
		}

		/// \brief retrieves the value T from the ScriptObject by casting it to a GetValueObject<T>
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

		/// \brief helper function to create a ScriptObjectArray (used if first is a ScriptObject)
		template<class TFirst, class... TRest>
		static std::enable_if_t<std::is_convertible_v<TFirst, ScriptObjectPtr> && !std::is_same_v<std::nullptr_t, TFirst>> 
		makeArray(ScriptObjectArray& array, const TFirst& first, const TRest&... objects)
		{
			array.add(first);
			makeArray(array, objects...);
		}

		/// \brief helper function to create a ScriptObjectArray (used if first is not a ScriptObject)
		template<class TFirst, class... TRest>
		static std::enable_if_t<!std::is_convertible_v<TFirst, ScriptObjectPtr> || std::is_same_v<std::nullptr_t, TFirst>>
			makeArray(ScriptObjectArray& array, const TFirst& first, const TRest&... objects)
		{
			array.add(toScriptObject(first));
			makeArray(array, objects...);
		}

		/// \brief recursion end of the helper function
		static void makeArray(ScriptObjectArray&) {}
	};
}
