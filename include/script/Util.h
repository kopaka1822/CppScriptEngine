#pragma once
#include "objects/ScriptObject.h"
#include "objects/ArrayObject.h"
#include "objects/ValueObject.h"
#include "Exception.h"
#include <sstream>
#include "objects/NullObject.h"
#include <set>
#include <algorithm>

namespace script
{
	class Util final
	{
	public:
		Util() = delete;

		/// \brief converts a class member function into a ScriptObject compatible function (which returns the parent ScriptObject)
		/// \tparam TClass 
		/// \tparam TArgs 
		/// \tparam TReturn this value will be ignored
		/// \param parent pointer of the class that owns this function
		/// \param thisPtr pointer for the member function
		/// \param func member function pointer
		/// \param functionSignature syntax: className::functionName(type1 name, type2...)
		/// \return ScriptObject compatible function
		template<class TClass, class TReturn, class... TArgs>
		static ScriptObject::FunctionT makeFunction(ScriptObject* parent, TClass* thisPtr, TReturn(TClass::* func)(TArgs...), const std::string& functionSignature)
		{
			return [thisPtr, func, functionSignature, parent]
			(const ArrayObjectPtr& args) -> ScriptObjectPtr
			{
				const size_t argCount = std::tuple_size<std::tuple<TArgs...>>::value;
				if (int(argCount) != args->getCount())
					throw InvalidArgumentCount(functionSignature, argCount, args->getCount());

				Util::invokeArgs(thisPtr, func, *args, std::index_sequence_for<TArgs...>{}, functionSignature);
				return parent->shared_from_this();
			};
		}

		/// \brief converts a class member function into a ScriptObject compatible function (which returns the parent ScriptObject)
		/// same as above but with a constant function
		/// \tparam TClass 
		/// \tparam TArgs 
		/// \tparam TReturn this value will be ignored
		/// \param parent pointer of the class that owns this function
		/// \param thisPtr pointer for the member function
		/// \param func member function pointer
		/// \param functionSignature syntax: className::functionName(type1 name, type2...)
		/// \return ScriptObject compatible function
		template<class TClass, class TReturn, class... TArgs>
		static ScriptObject::FunctionT makeFunction(ScriptObject* parent, const TClass* thisPtr, TReturn(TClass::* func)(TArgs...) const, const std::string& functionSignature)
		{
			// its okay because this and function were const => no change will happen
			return Util::makeFunction(parent, const_cast<TClass*>(thisPtr), reinterpret_cast<void(TClass::*)(TArgs...)>(func), functionSignature);
		}

		/// \brief converts a class member function into a ScriptObject compatible function
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

		/// \brief converts a class member function into a ScriptObject compatible function
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

		/// \brief converts a class member function into a ScriptObject compatible function
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
				if (int(argCount) != args->getCount())
					throw InvalidArgumentCount(functionSignature, argCount, args->getCount());

				if constexpr (std::is_convertible<TReturn, ScriptObjectPtr>::value)
					// return type is already a script object
					return Util::invokeArgs(thisPtr, func, *args, std::index_sequence_for<TArgs...>{}, functionSignature);
				else
					// return type must be transformed to a script object
					return Util::makeObject(Util::invokeArgs(thisPtr, func, *args, std::index_sequence_for<TArgs...>{}, functionSignature));
			};
		}

		/// \brief converts a const class member function into a ScriptObject compatible function
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

		/// \brief converts a static function into a ScriptObject compatible function
		/// \tparam TReturn must be convertible to a ScriptObject
		/// \tparam TArgs 
		/// \param func static function
		/// \param functionSignature returnType className::functionName(type1 name, type2...)
		/// \return ScriptObject compatible function
		template<class TReturn, class... TArgs>
		static ScriptObject::FunctionT makeFunction(TReturn(*func)(TArgs...), const std::string& functionSignature)
		{
			return [func, functionSignature](const ArrayObjectPtr& args) -> ScriptObjectPtr
			{
				const size_t argCount = std::tuple_size<std::tuple<TArgs...>>::value;
				if (int(argCount) != args->getCount())
					throw InvalidArgumentCount(functionSignature, argCount, args->getCount());

				if constexpr (std::is_convertible<TReturn, ScriptObjectPtr>::value)
					// return type is already a script object
					return Util::invokeArgs(func, *args, std::index_sequence_for<TArgs...>{}, functionSignature);
				else
					// return type must be transformed to a script object
					return Util::makeObject(Util::invokeArgs(func, *args, std::index_sequence_for<TArgs...>{}, functionSignature));
			};
		}

		template<class TLambda>
		static ScriptObject::FunctionT fromLambda(const TLambda& lambda, const std::string& functionSignature)
		{
			return makeFunction(&lambda, &TLambda::operator(), functionSignature);
		}

		/// \brief merges multiple functions into one.
		/// The functions will be called in order up to the first function that does not throw an
		/// InvalidArgumentCount or InvalidArgumentType exception.
		/// \param functions 
		/// \return return value of the first matching function
		static ScriptObject::FunctionT combineFunctions(std::initializer_list<ScriptObject::FunctionT> functions)
		{
			return [funcs = std::vector<ScriptObject::FunctionT>(functions)](const ArrayObjectPtr& args) -> ScriptObjectPtr
			{
				// keep track of errors
				std::set<size_t> supportedCounts;
				std::set<std::string> signatures;
				std::string invalidTypeErrors;

				// start with the first function
				for(const auto& func : funcs)
				{
					try
					{
						return func(args);
					}
					catch (const InvalidArgumentCount& e)
					{
						// remember error details
						for (const auto& fs : e.functionSignatures)
							signatures.insert(fs);
						for (const auto& num : e.expectedCounts)
							supportedCounts.insert(num);
					}
					catch (const InvalidArgumentType& e)
					{
						if (!invalidTypeErrors.empty())
							invalidTypeErrors += "; ";
						invalidTypeErrors += e.what();
					}
				}

				// found matching functions but arguments were wrong...
				if (!invalidTypeErrors.empty())
					throw InvalidArgumentType(invalidTypeErrors);

				// didn't find a matching function concerning arguments count
				throw InvalidArgumentCount(move(signatures), move(supportedCounts), args->getCount());
			};
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
		/// this is used to convert basic return types of functions to ScriptObjects
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

		/// \brief converts the description from typeid(T).name() in a prettier description
		static std::string prettyTypeName(std::string name)
		{
			if (name == typeid(std::string).name())
				return "std::string";

			// starts with class?
			if (name.rfind("class ", 0) == 0)
				name = name.substr(6);

			// starts with struct
			if (name.rfind("struct ", 0) == 0)
				name = name.substr(7);

			return name;
		}

		/// \brief calls toString on the object. Returns the string without "" if it is a StringObject
		static std::string getBareString(const ScriptObjectPtr& object)
		{
			auto strObj = std::dynamic_pointer_cast<GetValueObject<std::string>>(object);
			if (strObj)
			{
				return strObj->getValue();
			}
			return object->toString();
		}
	private:
		// helper functions to remove the shared_ptr wrapper
		template<class T> struct remove_shared { typedef T type; };
		template<class T> struct remove_shared<std::shared_ptr<T>> { typedef T type; };
		template<class T> struct remove_shared<std::shared_ptr<T>&> { typedef T type; };
		template<class T> struct remove_shared<const std::shared_ptr<T>> { typedef T type; };
		template<class T> struct remove_shared<const std::shared_ptr<T>&> { typedef T type; };
		template<class T> using remove_shared_t = typename  remove_shared<T>::type;

		template<class T> struct is_shared { static constexpr bool value = false; };
		template<class T> struct is_shared<std::shared_ptr<T>> { static constexpr bool value = true; };
		template<class T> struct is_shared<std::shared_ptr<T>&> { static constexpr bool value = true; };
		template<class T> struct is_shared<const std::shared_ptr<T>> { static constexpr bool value = true; };
		template<class T> struct is_shared<const std::shared_ptr<T>&> { static constexpr bool value = true; };
		template<class T> static constexpr bool is_shared_v = is_shared<T>::value;

		template<class T> using remove_const_ref_t = std::remove_const_t<std::remove_reference_t<T>>;

		template<class T> static constexpr bool is_shared_script_object_v = is_shared_v<T> && std::is_base_of_v<ScriptObject, remove_shared<T>>;

		/// \brief helper function to call unpack arg with the appropriate indices from the index sequence
		template<class TClass, class TReturn, class... TArgs, size_t... Is>
		static TReturn invokeArgs(TClass* thisPtr, TReturn(TClass::* func)(TArgs...), const ArrayObject& args, std::index_sequence<Is...>, const std::string& functionSignature)
		{
			return std::invoke(func, thisPtr, Util::unpackArg<TArgs>(args, Is, functionSignature)...);
		}

		/// \brief helper function to call unpack arg with the appropriate indices from the index sequence
		template<class TReturn, class... TArgs, size_t... Is>
		static TReturn invokeArgs(TReturn(*func)(TArgs...), const ArrayObject& args, std::index_sequence<Is...>, const std::string& functionSignature)
		{
			return std::invoke(func, Util::unpackArg<TArgs>(args, Is, functionSignature)...);
		}

		/// \brief helper function to call unpack arg with the appropriate indices from the index sequence
		template<class TReturn, class... TArgs, size_t... Is>
		static TReturn invokeArgs(std::function<TReturn(TArgs...)> func, const ArrayObject& args, std::index_sequence<Is...>, const std::string& functionSignature)
		{
			return func(Util::unpackArg<TArgs>(args, Is, functionSignature)...);
		}

		/// \brief converts the argument at args->get(argCount - 1) to T where T is a shared_ptr<ScriptObject>
		template<class T>
		static std::remove_reference_t<T> unpackArg(const ArrayObject& args, size_t index, const std::string& functionSignature,
			// convertible to ScriptObjectPtr
			std::enable_if_t<std::is_convertible_v<T, ScriptObjectPtr>, int> = 0)
		{
			const auto& objectPtr = args.get(int(index));

			// T is a base of ScriptObject
			// try to convert objectPtr to T
			using bare_type = remove_shared_t<T>;
			std::shared_ptr<bare_type> res = std::dynamic_pointer_cast<bare_type>(objectPtr);
			if (!res)
				throw InvalidArgumentType(functionSignature, index, *objectPtr, prettyTypeName(typeid(bare_type).name()));

			return res;
		}

		template<class T>
		static T& unpackArg(const ArrayObject& args, size_t index, const std::string& functionSignature,
			// convertible to ScriptObject
			std::enable_if_t<std::is_base_of_v<ScriptObject, remove_const_ref_t<T>>, int> = 0)
		{
			using bare_type = remove_const_ref_t<T>;

			auto objPtr = args.get(int(index)).get();
			auto val = dynamic_cast<bare_type*>(objPtr);
			if (val == nullptr)
				throw InvalidArgumentType(functionSignature, index, *objPtr, prettyTypeName(typeid(bare_type).name()));

			return *val;
		}

		/// \brief converts the argument at args->get(argCount - 1) to T if the argument has the type GetValueObject<T>
		template<class T>
		static T& unpackArg(const ArrayObject& args, size_t index, const std::string& functionSignature, 
			// not convertible to ScriptObjectPtr and not pointer
			std::enable_if_t<!std::is_convertible_v<T, ScriptObjectPtr> && !std::is_base_of_v<ScriptObject, remove_const_ref_t<T>> && !std::is_pointer_v<T>, int> = 0)
		{
			const auto& objectPtr = args.get(int(index));

			return Util::getGetValueObjectValue<T>(objectPtr.get(), index, functionSignature);
		}

		/// \brief converts the argument at args->get(argCount - 1) to T* if the argument has the type GetValueObject<T> or NullObject
		template<class T>
		static T unpackArg(const ArrayObject& args, size_t index, const std::string& functionSignature, 
			// not convertible to ScriptObjectPtr but pointer
			std::enable_if_t<std::is_pointer_v<T>, int> = 0)
		{
			const auto& objectPtr = args.get(int(index));

			// is nullptr?
			if (objectPtr->equals(NullObject::get())) return nullptr;

			// remove pointer
			using value_type = std::remove_pointer_t<T>;

			return &(Util::getGetValueObjectValue<value_type>(objectPtr.get(), index, functionSignature));
		}

		/// \brief retrieves the value T from the ScriptObject by casting it to a GetValueObject<T>
		template<class T>
		static T& getGetValueObjectValue(ScriptObject* object, size_t index, const std::string& functionSignature)
		{
			using bare_type = remove_const_ref_t<T>;

			if constexpr(std::is_base_of_v<ScriptObject, bare_type>)
			{
				// try dynamic up casting
				auto* valuePtr = dynamic_cast<bare_type*>(object);

				if (valuePtr == nullptr)
					throw InvalidArgumentType(functionSignature, index, *object, prettyTypeName(typeid(bare_type).name()));

				return *valuePtr;
			}
			else // embedded into value object?
			{
				// cast to value object
				// and transform const T& to T
				auto* valuePtr = dynamic_cast<GetValueObject<bare_type>*>(object); // T& => T

				if (valuePtr == nullptr)
					throw InvalidArgumentType(functionSignature, index, *object, prettyTypeName(typeid(bare_type).name()));

				return valuePtr->getValue();
			}
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
