# Utility

The static `script::Util` class provides a lot of useful functions to convert C++ functions into functions that can be used by the `ScriptEngine`.

## Contents

* [`Util::makeFunction(...)`](#makefunction)
* [`Util::combineFunctions(...)`](#combinefunctions)
* [`Util::makeObject(...)`](#makeobject)
* [`Util::makeArray(...)`](#makearray)
* [`Util::getBareString(...)`](getbarestring)

## MakeFunction

These functions are used to convert a single function into the `ScriptObject::FunctionT` format. `ScriptObject::FunctionT``is defined as: `std::function<ScriptObjectPtr(const ScriptPtr<ArrayObject>&)>`.
  
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

### Static Functions

```c++
1) template<class TReturn, class... TArgs>
   FunctionT makeFunction(TReturn(*func)(TArgs...), string functionSignature);
   
2) template<class TLambda>
   FunctionT fromLambda(const TLambda& lambda, string functionSignature);
```

## CombineFunctions

## MakeObject

## MakeArray

## GetBareString
