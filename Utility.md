# Utility

The static `script::Util` class provides a lot of useful functions to convert C++ functions into functions that can be used by the `ScriptEngine`.

## Contents

* [`Util::makeFunction(...)`](#makefunction)
* [`Util::combineFunctions(...)`](#combinefunctions)
* [`Util::makeObject(...)`](#makeobject)
* [`Util::makeArray(...)`](#makearray)
* [`Util::getBareString(...)`](#getbarestring)

## MakeFunction

These functions are used to convert a single function into the `ScriptObject::FunctionT` format. `ScriptObject::FunctionT` is defined as: `std::function<ScriptObjectPtr(const ScriptPtr<ArrayObject>&)>`. Those functions also perform argument count checking and type checking for all arguments.

If the number of provided arguments does not match the expected argument count, an `InvalidArgumentCount` exception is thrown.

If an argument is not convertible to the expected argument type, an `InvalidArgumentType` exception is thown. Arguments are considered convertible to the argument type if:
* the provided argument is directly convertible to the expected argument. (expected `ScriptPtr<IntObject>` and got `ScriptPtr<IntObject>`)
* the provided argument has the type `GetValueObject<T>` with `T` as expected argument (an `IntObject` derives from `GetValueObject<int>` and is therefore convertible to a c++ `int`).
* One of the above cases, but the expected argument is a raw pointer. In this case, the argument `NullObject` is also valid and a value of `nullptr` will be used for the function call. (if the expected argument is a `int*` then arguments of type `IntObject` and `NullObject` are permitted)
 
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

This function allows the combination of multiple `FunctionT` functions into a single `FunctionT` function. Example for creating two different constructors for the `Vec2` class:

```c++
Util::combineFunctions({
   Util::fromLambda([](const Vec2& other){
      return std::make_shared<Vec2>(other.getX(), other.getY());
   }, "Vec2(Vec2)"),
   script::Util::fromLambda([](float x, float y){
      return std::make_shared<Vec2>(x, y);
   }, "Vec2(float x, float y)")
   });
```

#### Behaviour
This function starts with calling the first provided function. If either an `InvalidArgumentType` or `InvalidArgumentCount` is thrown the function will be considered a *unmatching* and the next function in the list will be called. If the function returns normally or throws another type of error, the function will be considered a *match* and no other function will be called. If no *matching* functions were called one of the following exceptions will be thrown:
* `InvalidArgumentCount` if *all* functions failed because of an `InvalidArgumentCount` exception.
* `InvalidArgumentType` otherwise. This exception will contain the error information of all functions that threw an `InvalidArgumentType` exception.

## MakeObject

This function can be used to create `ScriptObjectPtr` from another object. The function signature is:

```c++
template<class T>
static ScriptObjectPtr makeObject(const T& value);
```

The `ScriptEngine` provides implementations for:
* `makeObject<int>`
* `makeObject<float>`
* `makeObject<bool>`
* `makeObject<std::string>`
* `makeObject<const char*>`
* `makeObject<nullptr>`
* `makeObject<std::vector<T>>` (`makeObject` will be called again for each argument)

This function is used by the `Util::makeFunction` functions that return a type that is not trivially convertible to a `ScriptObjectPtr`

## MakeArray

This functions converts all arguments into a `ScriptObjectPtr` and return a `ArrayObjectPtr` (`ScriptPtr<ArrayObject>`).
```c++
template<class... TClass>
static ArrayObjectPtr makeArray(const TClass&... objects)
```

## GetBareString

This function calls `toString()` on the provided argument and returns its value. However, if the object is a `StringObject` (or a `GetValueObject<string>`) the `getValue()` function will be used instead if the `toString()` function.

This function was introduced to avoid the additional quotation marks that will be added by the `StringObject` when retrieving a string representation.
