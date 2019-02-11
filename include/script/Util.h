#pragma once
#include "objects/ScriptObject.h"
#include "objects/ArrayObject.h"
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
		static ScriptObject::FunctionT makeFunction(ScriptObject* parent, TClass* thisPtr, void(TClass::* func)(TArgs...), const std::string& functionSignature)
		{
			return [thisPtr, func, functionSignature, parent]
			(const ArrayObjectPtr& args) -> ScriptObjectPtr
			{
				const size_t argCount = std::tuple_size<std::tuple<TArgs...>>::value;
				if (int(argCount) != args->count())
					throw InvalidArgumentCount(functionSignature, argCount, args);

				Util::invokeArgs(thisPtr, func, *args, std::index_sequence_for<TArgs...>{}, functionSignature);
				return parent->shared_from_this();
			};
		}

		/// \brief converts a class memberfunction into a ScriptObject compatible function
		/// same as above but with a constant function
		/// \tparam TClass 
		/// \tparam TArgs 
		/// \param parent pointer of the class that owns this function
		/// \param thisPtr pointer for the member function
		/// \param func member function pointer
		/// \param functionSignature syntax: className::functionName(type1 name, type2...)
		/// \return ScriptObject compatible function
		template<class TClass, class... TArgs>
		static ScriptObject::FunctionT makeFunction(ScriptObject* parent, const TClass* thisPtr, void(TClass::* func)(TArgs...) const, const std::string& functionSignature)
		{
			// its okay because this and function were const => no change will happen
			return Util::makeFunction(parent, const_cast<TClass*>(thisPtr), reinterpret_cast<void(TClass::*)(TArgs...)>(func), functionSignature);
		}

		/// \brief converts a class memberfunction into a ScriptObject compatible function
		/// \tparam TClass must be convertible to ScriptObject
		/// \tparam TArgs 
		/// \param thisPtr class pointer
		/// \param func member function pointer
		/// \param functionSignature syntax: className::functionName(type1 name, type2...)
		/// \return ScriptObject compatible function
		template<class TClass, class... TArgs>
		static ScriptObject::FunctionT makeFunction(TClass* thisPtr, void(TClass::* func)(TArgs...), const std::string& functionSignature)
		{
			static_assert(std::is_base_of_v<ScriptObject, TClass>, "the first argument must be derived from ScriptObject");
			return Util::makeFunction(static_cast<ScriptObject*>(thisPtr), thisPtr, func, functionSignature);
		}

		/// \brief converts a class memberfunction into a ScriptObject compatible function
		/// same as above but with a constant function
		/// \tparam TClass must be convertible to ScriptObject
		/// \tparam TArgs 
		/// \param thisPtr class pointer
		/// \param func member function pointer
		/// \param functionSignature syntax: className::functionName(type1 name, type2...)
		/// \return ScriptObject compatible function
		template<class TClass, class... TArgs>
		static ScriptObject::FunctionT makeFunction(const TClass* thisPtr, void(TClass::* func)(TArgs...) const, const std::string& functionSignature)
		{
			// its okay because this and function were const => no change will happen
			return Util::makeFunction(const_cast<TClass*>(thisPtr), reinterpret_cast<void(TClass::*)(TArgs...)>(func), functionSignature);
		}

		/// \brief converts a class memberfunction into a ScriptObject compatible function
		/// \tparam TClass 
		/// \tparam TArgs 
		/// \param thisPtr class pointer
		/// \param func member function pointer
		/// \param functionSignature syntax: returnType className::functionName(type1 name, type2...)
		/// \return ScriptObject compatible function
		template<class TClass, class TReturn, class... TArgs>
		static ScriptObject::FunctionT makeFunction(TClass* thisPtr, TReturn(TClass::* func)(TArgs...), const std::string& functionSignature)
		{
			return [thisPtr, func, functionSignature]
			(const ArrayObjectPtr& args) -> ScriptObjectPtr
			{
				const size_t argCount = std::tuple_size<std::tuple<TArgs...>>::value;
				if (int(argCount) != args->count())
					throw InvalidArgumentCount(functionSignature, argCount, args);

				if constexpr (std::is_convertible<TReturn, ScriptObjectPtr>::value)
					// return type is already a script object
					return Util::invokeArgs(thisPtr, func, *args, std::index_sequence_for<TArgs...>{}, functionSignature);
				else
					// return type must be transformed to a script object
					return Util::makeObject(Util::invokeArgs(thisPtr, func, *args, std::index_sequence_for<TArgs...>{}, functionSignature));
			};
		}

		/// \brief converts a const class memberfunction into a ScriptObject compatible function
		/// \tparam TClass 
		/// \tparam TArgs 
		/// \param thisPtr class pointer
		/// \param func member function pointer
		/// \param functionSignature syntax: returnType className::functionName(type1 name, type2...)
		/// \return ScriptObject compatible function
		template<class TClass, class TReturn, class... TArgs>
		static ScriptObject::FunctionT makeFunction(const TClass* thisPtr, TReturn(TClass::* func)(TArgs...) const, const std::string& functionSignature)
		{
			// its okay because this and function were const => no change will happen
			return makeFunction(const_cast<TClass*>(thisPtr), reinterpret_cast<TReturn(TClass::*)(TArgs...)>(func), functionSignature);
		}

		/// \brief puts the arguments into a ArrayObjectPtr
		template<class... TClass>
		static ArrayObjectPtr makeArray(const TClass&... objects)
		{
			auto arr = std::make_shared<ArrayObject>();
			makeArray(*arr, objects...);
			return arr;
		}

		/// \brief converts the T value into a GetValueObject<T>
		/// this is used to convert basic return types of functions to SciptObjects
		template<class T>
		static ScriptObjectPtr makeObject(const T& value);

		/// \brief calls makeObject with an std::string parameter
		static ScriptObjectPtr makeObject(const char* text);

		/// \brief converts an std vector into an array object
		/// \tparam T must be convertible to a ScriptObject or have an appropriate makeObject function
		template<class T>
		static ArrayObjectPtr makeObject(const std::vector<T>& vec)
		{
			std::vector<ScriptObjectPtr> res;
			res.reserve(vec.size());
			for(const auto& e : vec)
			{
				if constexpr (std::is_convertible_v<T, ScriptObjectPtr>)
					res.push_back(e);
				else
					res.push_back(Util::makeObject(e));
			}
			return std::make_shared<ArrayObject>(res);
		}
	private:
		// helper functions to remove the shared_ptr wrapper
		template<class T> struct remove_shared { typedef T type; };
		template<class T> struct remove_shared<std::shared_ptr<T>> { typedef T type; };
		template<class T> struct remove_shared<std::shared_ptr<T>&> { typedef T type; };
		template<class T> struct remove_shared<const std::shared_ptr<T>> { typedef T type; };
		template<class T> struct remove_shared<const std::shared_ptr<T>&> { typedef T type; };
		template<class T> using remove_shared_t = typename  remove_shared<T>::type;

		/// \brief helper function to call unpack arg with the appropriate indices from the index sequence
		template<class TClass, class TReturn, class... TArgs, size_t... Is>
		static TReturn invokeArgs(TClass* thisPtr, TReturn(TClass::* func)(TArgs...), const ArrayObject& args, std::index_sequence<Is...>, std::string functionSignature)
		{
			return std::invoke(func, thisPtr, Util::unpackArg<TArgs>(args, Is, functionSignature)...);
		}

		/// \brief converts the argument at args->get(argCount - 1) to T where T is a shared_ptr<ScriptObject>
		template<class T>
		static std::remove_reference_t<T> unpackArg(const ArrayObject& args, size_t index, const std::string& functionSignature,
			// convertible to ScriptObjectPtr
			std::enable_if_t<std::is_convertible<T, ScriptObjectPtr>::value, int> = 0)
		{
			const auto& objectPtr = args.get(index);

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
		static T& unpackArg(const ArrayObject& args, size_t index, const std::string& functionSignature, 
			// not convertible to ScriptObjectPtr and not pointer
			std::enable_if_t<!std::is_convertible<T, ScriptObjectPtr>::value, int> = 0, std::enable_if_t<!std::is_pointer<T>::value, int> = 0)
		{
			const auto& objectPtr = args.get(index);

			return Util::getGetValueObjectValue<T>(objectPtr.get(), args, index, functionSignature);
		}

		/// \brief converts the argument at args->get(argCount - 1) to T* if the argument has the type GetValueObject<T> or NullObject
		template<class T>
		static T unpackArg(const ArrayObject& args, size_t index, const std::string& functionSignature, 
			// not convertible to ScriptObjectPtr but pointer
			std::enable_if_t<!std::is_convertible<T, ScriptObjectPtr>::value, int> = 0, std::enable_if_t<std::is_pointer<T>::value, int> = 0)
		{
			const auto& objectPtr = args.get(index);

			// is nullptr?
			if (objectPtr->equals(NullObject::get())) return nullptr;

			// remove pointer
			using value_type = std::remove_pointer_t<T>;

			return &(Util::getGetValueObjectValue<value_type>(objectPtr.get(), args, index, functionSignature));
		}

		/// \brief retrieves the value T from the ScriptObject by casting it to a GetValueObject<T>
		template<class T>
		static T& getGetValueObjectValue(ScriptObject* object, const ArrayObject& args, size_t index, const std::string& functionSignature)
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

		/// \brief helper function to create a ArrayObject (used if first is a ScriptObject)
		template<class TFirst, class... TRest>
		static std::enable_if_t<std::is_convertible_v<TFirst, ScriptObjectPtr> && !std::is_same_v<std::nullptr_t, TFirst>> 
		makeArray(ArrayObject& array, const TFirst& first, const TRest&... objects)
		{
			array.add(first);
			Util::makeArray(array, objects...);
		}

		/// \brief helper function to create a ArrayObject (used if first is not a ScriptObject)
		template<class TFirst, class... TRest>
		static std::enable_if_t<!std::is_convertible_v<TFirst, ScriptObjectPtr> || std::is_same_v<std::nullptr_t, TFirst>>
			makeArray(ArrayObject& array, const TFirst& first, const TRest&... objects)
		{
			array.add(Util::makeObject(first));
			Util::makeArray(array, objects...);
		}

		/// \brief recursion end of the helper function
		static void makeArray(ArrayObject&) {}
	};
}
