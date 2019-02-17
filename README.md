# CppScriptEngine

C++ based object oriented script language

## Motivation

This script language should allow the programmer to control a programm by creating new instances of **C++ classes at runtime**. The language should be easy to use **easy to integrate** into an existing C++ programm.

# Language Design

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
