# CppScriptEngine

C++ 17 based object oriented script language

## Motivation

This script language should allow the programmer to control a programm by creating new instances of **C++ classes at runtime**. The language should be easy to use and **easy to integrate** into an existing C++ programm.

## Goals
* easy to use
* easy to integrate
* consistent design (no special cases)
* fixed naming conventions
* strict syntax
* type safety without implicit conversions
* robust
* well informing exceptions

# Introduction
## ScriptObject

Every object derives from the `ScriptObject` class. The important part of the interface looks like this:
```c++
class ScriptObject {
public:
   virtual std::string toString() const;
   virtual ScriptObjectPtr clone() const;
   virtual bool equals(const ScriptObjectPtr& other) const;

   ScriptObjectPtr invoke(const std::string& funcName, const ScriptPtr<ArrayObject>& args);
protected:
   using FunctionT = std::function<ScriptObjectPtr(const ScriptPtr<ArrayObject>&)>;
   void addFunction(const std::string& name, const FunctionT& func);
}
```

* `toString()` returns the string representation. The default implementation returns the class name.
* `clone()` does a **deep copy** of the object. The default implementation throws an `ObjectNotCloneableException`.
* `equals(other)` tests for equality with another object. The default implementation checks for reference equality.
* `invoke(name, args)` invokes the function with the given names and passes on the arguments.
* `addFunction(name, func)` registers a new function which can be called by using `invoke` with the specified name.

## Predefined objects

Some primitives are already implemented by the following classes:
* `IntObject` represents a C++ `int`. Usage: `i = 24`
* `FloatObject` represents a C++ `float`. Usage: `f = 24.0f`
* `BoolObject` represents a C++ `bool`. Usage: `b = true`
* `StringObject` represents a C++ `std::string`. Usage: `s = "test"`
* `NullObject` represents a C++ `nullptr`. Usage: `n = null`
* `ArrayObject` represents an array of `ScriptObject`. Usage `a = [1, 1.0f, "test"]`

## Functions

Functions that were registered with `addFunction` can be called direcly from the script. The `IntObject` and `FloatObject` classes already have some basic functionality.
* **Functions** are designed to be **mutators**. That means that a function should modify its value instead of returning a modified instance.

Example of some `IntObject` functions:
```
i = 3.add(6)
>> 9
i = 10.add(i.multiply(2))
>> 28
i.subtract(20)
>> 8
i
>> 8
i.getFunctionNames()
>> ["clone", "toString", "multiply", "getFunctionNames", "invoke", "add", "equals", "subtract", "divide", "negate", "set"]
```
# Naming Conventions
## Naming Conventions
Type | Convention
-----|-----------
Class Names | UpperCamelCase
Member Functions | lowerCamelCase(...)
Getter | getCamelCase
Setter | setCamelCase = ...
Static Functions / Constructors | UpperCamelCase(...)
variables | lowerCamelCase
Static objects | UpperCamelCase

Additionally functions and objects **must** start with a letter and may be continued with alphanumeric characters.

Reserved Keyword | Type
-----------------|-----
true | BoolObject
false | BoolObject
null | NullObject
*number* | IntObject
*number ending with the letter f* | FloatObject
"*some text*" | StringObject
\[*some, elements*\] | ArrayObject

Note that the script engine won't recognize certain commands if you do not follow those naming conventions. For example: You cannot create a variable that starts with an uppercase letter. The script engine assumes that all variables starting with an uppercase letter are static objects (which can not be assigned though a command). 

# Syntactic Sugar
## Operators

In order to make basic math operations easier to use, the following operators can be used as well: `+ - * /` and `+= -= *= /=`.

The usage of an assignment operator translates from `a += c` to `a.add(c)`. Note that this stores the result in `a` because the function is a **mutator**.

The other operators are translated from `a = b * c` to `a = b.clone().multiply(c)`. Note that `clone` is called before calling the operator function to avoid changing the value of `b`.

Example from before modified:
```
i = 3 + 6
>> 9
i = 10 + i * 2
>> 28
i -= 20
>> 8
i
>> 8
```

## Properties
Functions with a get/set prefix can be called by skipping the get/set and the brackets of the function under the following conditions:
* **getter functions** should not require any arguments
* **setter functions** should required exactly one argument
Example:
```
i.FunctionNames
>> ["clone", "toString", "multiply", "getFunctionNames", "invoke", "add", "equals", "subtract", "divide", "negate", "set"]
obj.SomeSetter = value
>> ...
```
