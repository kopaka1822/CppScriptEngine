#pragma once
#include "objects/ScriptObject.h"
#include "objects/ScriptObjectArray.h"
#include "objects/ValueObject.h"
#include <sstream>

namespace script
{
	class Util final
	{
	public:
		Util() = delete;

		template<class TClass, class... TArgs>
		static std::function<ScriptObjectPtr(ScriptObjectArrayPtr)> makeFunction(TClass* thisPtr, void(TClass::* func)(TArgs...), std::string functionSignature = "")
		{
			auto errorMessage = functionSignature.empty() ? "" : "for " + functionSignature;
			std::function<ScriptObjectPtr(ScriptObjectArrayPtr)> res = (
				[thisPtr, func, errorMessage]
			(ScriptObjectArrayPtr args) -> ScriptObjectPtr
			{
				size_t argCount = std::tuple_size<std::tuple<TArgs...>>::value;
				if (argCount != args->count())
				{
					std::stringstream ss;
					ss << "invalid argument count " << errorMessage
						<< ". expected " << argCount << " but got " << args->count()
						<< ". args: " + args->toString();
					
					throw std::runtime_error(ss.str());
				}

				std::invoke(func, thisPtr, unpackArg<TArgs>(args, argCount, errorMessage)...);
				return dynamic_cast<ScriptObject*>(thisPtr)->shared_from_this();
			});
			return res;
		}

	private:
		template<class T>
		static const T& unpackArg(ScriptObjectArrayPtr& args, size_t& argCount, const std::string& errorMessage)
		{
			const auto index = --argCount;
			const auto& objectPtr = args->get(index);

			// cast to value object
			// and transform const T& to T
			const auto* valuePtr = dynamic_cast<ValueObject<
				typename std::remove_const< // const T => T
				typename std::remove_reference<T>::type>::type>*> // T& => T
				(objectPtr.get());

			if (valuePtr == nullptr)
			{
				std::stringstream ss;
				ss << "invalid argument type for argument " << index << " ("
					<< objectPtr->toString() << ") " << errorMessage
					<< ". args: " + args->toString();
				throw std::runtime_error(ss.str());
				
			}
				

			return valuePtr->getValue();
		}
	};
}
