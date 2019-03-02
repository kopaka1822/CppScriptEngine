# Utility

The static `script::Util` class provides a lot of useful functions to convert C++ functions into functions that can be used by the `ScriptEngine`.

## Contents

* [`Util::makeFunction(...)`](#makefunction)
* [`Util::combineFunctions(...)`](#combinefunctions)
* [`Util::makeObject(...)`](#makeobject)
* [`Util::makeArray(...)`](#makearray)
* [`Util::getBareString(...)`](getbarestring)

## MakeFunction

These functions are used to convert a single function into the `ScriptObject::FunctionT` format. `ScriptObject::FunctionT` is defined as: `std::function<ScriptObjectPtr(const ScriptPtr<ArrayObject>&)>`. Those functions also perform argument count checking and type checking for all arguments.

If the number of provided arguments does not match the expected argument count, an `InvalidArgumentCount` exception is thrown.

If an argument is not convertible to the expected argument type, an `InvalidArgumentType` exception is thown. Arguments are considered convertible to the argument type if:
* the provided argument is directly convertible to the expected argument.
* the provided argument has the type `GetValueObject<T>` with `T` as expected argument (an `IntObject` derived from `GetValueObject<int>` and is therefore convertible to a c++ `int`).
* One of the above cases, but the expected argument is a raw pointer. In this case, the argument `NullObject` is also valid and a value of `nullptr` will be used for the function call.
 
In both cases, the `functionSignature` parameter is used to identify where the error occured. The syntax for the function signature should be `returnType className::functionName(type1 name, type2...)` or `className::functionName(type1 name, type2...)` (for void return type functions).

#### Example
Creating a `FunctionT` function with type and argument count checking is quite tedious. This example shows how adding a simple `add` function for a `Vec2` class would look like without the `Util` functions:

```c++
addFunction("add", [this](const script::ArrayObjectPtr& args) {
   if (args->getCount() != 1)
      throw script::InvalidArgumentCount("Vec2::add(Vec2)", 1, args->getCount());
   // try to obtain vec2
   const script::ScriptObjectPtr& arg1 = args->get(0);
   Vec2* vecArg = dynamic_cast<Vec2*>(arg1.get());
   if (vecArg == nullptr)
      throw script::InvalidArgumentType("Vec2::add(Vec2)", 0, *arg1, "Vec2");

   // finally execute the function
   this->add(*vecArg);
   return this->shared_from_this();
});
```

The `Util` version looks like this:
```c++
addFunction("add", script::Util::makeFunction(this, &Vec2::add, "Vec2::add(Vec2)"));
```

### Class Member Functions (for ScriptObjects)
The following function templates are designed to convert member functions of `ScriptObject` derived classes:

```c++
1) template<class TClass, class... TArgs>
   FunctionT makeFunction(TClass* thisPtr, void(TClass::* func)(TArgs...), string functionSignature);

2) template<class TClass, class... TArgs>
   FunctionT makeFunction(const TClass* thisPtr, void(TClass::* func)(TArgs...) const, string functionSignature);

3) template<class TClass, class TReturn, class... TArgs>
   FunctionT makeFunction(TClass* thisPtr, TReturn(TClass::* func)(TArgs...), string functionSignature);

4) template<class TClass, class TReturn, class... TArgs>
   FunctionT makeFunction(const TClass* thisPtr, TReturn(TClass::* func)(TArgs...) const, string functionSignature);
```

1) This functions takes the class pointer followed by a member function that returns void. The return value of this function will be the `ScriptObjectPtr` of the class (which must be a `ScripObject`).
2) Same as 1 but with a `const` function.
3) This function takes the class pointer followed by a member function return non-void. The return value must be directly convertible to a `ScriptObjectPtr` or has an appropriate [`Util::makeObject(...)`](#makeobject) overload.
4) Same as 3 but with a `const` function.

### Class Member Functions (for non ScriptObjects)

```c++
1) template<class TClass, class TReturn, class... TArgs>
   FunctionT makeFunction(ScriptObject* parent, TClass* thisPtr, TReturn(TClass::* func)(TArgs...), string functionSignature);
   
2) template<class TClass, class TReturn, class... TArgs>
   FunctionT makeFunction(ScriptObject* parent, const TClass* thisPtr, TReturn(TClass::* func)(TArgs...) const, string functionSignature);
   
3) template<class TClass, class TReturn, class... TArgs>
   FunctionT makeFunction(TClass* thisPtr, TReturn(TClass::* func)(TArgs...), string functionSignature);

4) template<class TClass, class TReturn, class... TArgs>
   FunctionT makeFunction(const TClass* thisPtr, TReturn(TClass::* func)(TArgs...) const, string functionSignature);
```

1) This functions returns the `ScriptObject` provided by the first argument but executes the member function given by the second argument (and ignoring it's return value).
2) Same as 1 but with a `const` function.
3) Same as 3 from [Class Member Functions (for ScriptObjects)](#class-member-functions-for-scriptobjects)
4) Same as 4 from [Class Member Functions (for ScriptObjects)](#class-member-functions-for-scriptobjects)

### Static Functions

```c++
1) template<class TReturn, class... TArgs>
   FunctionT makeFunction(TReturn(*func)(TArgs...), string functionSignature);
   
2) template<class TLambda>
   FunctionT fromLambda(const TLambda& lambda, string functionSignature);
```

1) Converts a static function. The return value must be directly convertible to a `ScriptObjectPtr` or has an appropriate [`Util::makeObject(...)`](#makeobject) overload.
2) Converts a lambda function. The return value must be directly convertible to a `ScriptObjectPtr` or has an appropriate [`Util::makeObject(...)`](#makeobject) overload. This template only works if the lambda function is provided as the first argument without wrapping it into a `std::function` first.

## CombineFunctions

## MakeObject

## MakeArray

## GetBareString
