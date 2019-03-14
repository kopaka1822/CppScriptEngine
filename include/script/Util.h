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

#pragma region makeFunction

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
				Util::validateArgCount<TArgs...>(args, functionSignature);
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
				Util::validateArgCount<TArgs...>(args, functionSignature);
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
			return Util::makeFunction(const_cast<TClass*>(thisPtr), reinterpret_cast<TReturn(TClass::*)(TArgs...)>(func), functionSignature);
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
				Util::validateArgCount<TArgs...>(args, functionSignature);
				return Util::makeObject(Util::invokeArgs(func, *args, std::index_sequence_for<TArgs...>{}, functionSignature));
			};
		}

#pragma endregion

#pragma region fromLambda

		template<class TLambda>
		static ScriptObject::FunctionT fromLambda(TLambda lambda, const std::string& functionSignature)
		{
			return Util::fromLambdaOperator(lambda, &TLambda::operator(), functionSignature);
		}

		template<class TLambda>
		static ScriptObject::FunctionT fromLambda(TLambda& lambda, const std::string& functionSignature)
		{
			return Util::fromLambdaOperator(lambda, &TLambda::operator(), functionSignature);
		}

#pragma endregion

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

#pragma region makeObject

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
				res.push_back(Util::makeObject(e));
			}
			return std::make_shared<ArrayObject>(res);
		}

		/// \brief returns the script object if the pointer is valid. NullObject will be returned otherwise
		template<class T>
		static std::enable_if_t<std::is_base_of_v<ScriptObject, T>, 
		ScriptObjectPtr> makeObject(const std::shared_ptr<T>& obj)
		{
			if(obj) return obj;
			return NullObject::get();
		}

		/// \brief returns obj.shared_from_this()
		template<class T>
		static std::enable_if_t<std::is_base_of_v<ScriptObject, T>, 
		ScriptObjectPtr> makeObject(T& obj)
		{
			return obj.shared_from_this();
		}

#pragma endregion 

		
		/// \brief tries to convert a script object into the desired type. Throws an InvalidArgumentConversion on failure.
		/// \tparam T desired type
		/// \tparam allow_pointer reserved for internal use (pass true)
		/// \param obj script object that will be converted
		/// \return desired type
		template<class T, bool allow_pointer = true>
		// remove the reference from T if T is convertible to ScriptObjectPtr or a raw pointer (it would return a reference to a local variable)
		static std::conditional_t<std::is_convertible_v<T, ScriptObjectPtr> || std::is_pointer_v<T>, std::remove_reference_t<T>, T>
			fromObject(const ScriptObjectPtr& obj)
		{
			if constexpr (std::is_convertible_v<T, ScriptObjectPtr> && allow_pointer)
			{
				// T is a base of ScriptObject
				// try to convert objectPtr to T
				using bare_type = remove_shared_t<T>;
				std::shared_ptr<bare_type> res = std::dynamic_pointer_cast<bare_type>(obj);
				if (!res)
					throw InvalidArgumentConversion(prettyTypeName(typeid(bare_type).name()));

				return res;
			}
			else if constexpr (std::is_pointer_v<T> && allow_pointer)
			{
				// is nullptr?
				if (obj->equals(NullObject::get())) return nullptr;

				// remove pointer
				using value_type = std::remove_pointer_t<T>;

				return &(Util::fromObject<value_type&, false>(obj));
			}
			else
			{
				using bare_type = remove_const_ref_t<T>;

				// object derives from script object
				if constexpr (std::is_base_of_v<ScriptObject, bare_type>)
				{
					// try dynamic up casting
					auto* valuePtr = dynamic_cast<bare_type*>(obj.get());

					if (valuePtr == nullptr)
						throw InvalidArgumentConversion(prettyTypeName(typeid(bare_type).name()));

					return *valuePtr;
				}
				else // embedded into value object?
				{
					// cast to value object
					auto* valuePtr = dynamic_cast<GetValueObject<bare_type>*>(obj.get()); // T& => T

					if (valuePtr == nullptr)
						throw InvalidArgumentConversion(prettyTypeName(typeid(bare_type).name()));

					return valuePtr->getValue();
				}
			}
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

			// starts with enum
			if (name.rfind("enum ", 0) == 0)
				name = name.substr(5);

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
#pragma region fromLambda

		template<class TLambda, class TReturn, class... TArgs>
		static ScriptObject::FunctionT fromLambdaOperator(const TLambda& lambda, TReturn(TLambda::* func)(TArgs...) const, const std::string& functionSignature)
		{
			return Util::fromLambdaOperator(*const_cast<TLambda*>(&lambda), reinterpret_cast<TReturn(TLambda::*)(TArgs...)>(func), functionSignature);
		}

		// lambda function unpacker
		template<class TLambda, class TReturn, class... TArgs>
		static ScriptObject::FunctionT fromLambdaOperator(TLambda& lambda, TReturn(TLambda::*)(TArgs...), const std::string& functionSignature)
		{
			std::function<TReturn(TArgs...)> func = lambda;
			return [func, functionSignature](const ArrayObjectPtr& args) mutable -> ScriptObjectPtr
			{
				Util::validateArgCount<TArgs...>(args, functionSignature);
				return Util::makeObject(Util::invokeArgs(func, *args, std::index_sequence_for<TArgs...>{}, functionSignature));
			};
		}

#pragma endregion 

		// helper functions to remove the shared_ptr wrapper
		template<class T> struct remove_shared { typedef T type; };
		template<class T> struct remove_shared<std::shared_ptr<T>> { typedef T type; };
		template<class T> struct remove_shared<std::shared_ptr<T>&> { typedef T type; };
		template<class T> struct remove_shared<const std::shared_ptr<T>> { typedef T type; };
		template<class T> struct remove_shared<const std::shared_ptr<T>&> { typedef T type; };
		template<class T> using remove_shared_t = typename remove_shared<T>::type;

		// helper function to transform const T& into T
		template<class T> using remove_const_ref_t = std::remove_const_t<std::remove_reference_t<T>>;

#pragma region invokeArgs

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
		static TReturn invokeArgs(std::function<TReturn(TArgs...)>& func, const ArrayObject& args, std::index_sequence<Is...>, const std::string& functionSignature)
		{
			return func(Util::unpackArg<TArgs>(args, Is, functionSignature)...);
		}

#pragma endregion 

		template<class... TArgs>
		static void validateArgCount(const ArrayObjectPtr& args, const std::string& functionSignature)
		{
			const size_t argCount = std::tuple_size<std::tuple<TArgs...>>::value;
			if (int(argCount) != args->getCount())
				throw InvalidArgumentCount(functionSignature, argCount, args->getCount());
		}

		template<class T>
		// remove the reference from T if T is convertible to ScriptObjectPtr (it would returns a reference to a local variable)
		static std::conditional_t<std::is_convertible_v<T, ScriptObjectPtr>, std::remove_reference_t<T>, T>
		unpackArg(const ArrayObject& args, size_t index, const std::string& functionSignature)
		{
			const auto& objectPtr = args.get(int(index));

			try
			{
				return fromObject<T>(objectPtr);
			}
			catch (const InvalidArgumentConversion& e)
			{
				throw InvalidArgumentType(functionSignature, index, *objectPtr, e.desiredType);
			}
		}

#pragma region makeArray

		/// \brief helper function to create a ArrayObject
		template<class TFirst, class... TRest>
		static void makeArray(ArrayObject& array, const TFirst& first, const TRest&... objects)
		{
			array.add(Util::makeObject(first));
			Util::makeArray(array, objects...);
		}

		/// \brief recursion end of the helper function
		static void makeArray(ArrayObject&) {}

#pragma endregion
	};
}