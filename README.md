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

## Example Program
The following example program can be used to create the example from above.
```c++
#include <iostream>
#include <script/ScriptEngine.h>

int main() {
   script::ScriptEngine engine;
   
   std::string command;
   while (true) {
      // read console input
      std::getline(std::cin, command);
      try {
         // execute console input and print out result
         std::cout << ">> " << engine.execute(command) << std::endl;
      } catch (const std::exception& e) {
         std::cerr << "ERR: " << e.what() << std::endl;
      }
   }
   return 0;
}
```

# Adding Custom Objects
## Derive from GetValueObject
The easiest way to add custom objects is to derive directly from `GetValueObject<T>`.

### Example
This example explains how to write a simple Vector 2D class. In the end, you should be able to create a new instance of a `Vec2` and know how to register the member functions and use them with the script engine.

The header file for our `Vec2` class looks like this:

```c++
#include "script/objects/GetValueObject.h"

class Vec2 : public script::GetValueObject<Vec2> {
public:
   Vec2(float x, float y);
   // ScriptObject overrides
   std::string toString() const override;
   script::ScriptObjectPtr clone() const override;
   bool equals(const script::ScriptObjectPtr& other) const override;
   // GetValueObject<> override
   Vec2& getValue() override;
   // new functionality
   void add(const Vec2& other);
   void subtract(const Vec2& other);
   void negate();
   // properties
   float getX() const;
   float getY() const;
   void setX(float x);
   void setY(float y);
   // constructor
   static FunctionT getCtor();

   float x;
   float y;
};
```

### ScriptObject overrides
First of all, we want to overwrite the default behaviour of some `ScriptObjectFunctions`. The `toString` function should print out the x and y value:
```c++
std::string Vec2::toString() const {
   return "Vec2(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}
```
The equals function should do a memberwise value comparision (the default implementation is a reference comparision). Since the argument can be an arbitrary `ScriptObject` we have cast it to a `Vec2` object first.
```c++
bool Vec2::equals(const script::ScriptObjectPtr& other) const {
   const auto v = std::dynamic_pointer_cast<Vec2>(other);
   if (!v) return false; // cast failed
   return x == v->x && y == v->y;
}
```
The clone function simply creates a new `Vec2` with the same values. The `ScriptObjectPtr` is defined as a `std::shared_ptr<ScriptObject>`, thus we need to use `std::make_shared` to create a new instance.
```c++
script::ScriptObjectPtr Vec2::clone() const {
   return std::make_shared<Vec2>(x, y);
}
```
### GetValueObject override
The `getValue()` function will play an important part for some Utility functions (`script::Util`) that will be used for registering new functions for 'invoke(...)'. For now we just return a reference to itself:
```c++
Vec2& Vec2::getValue()
{
	return *this;
}
```

### New Functionality

### Registering Functions for Invoke

### Adding the Constructor

## Embed object with ValueObject

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
