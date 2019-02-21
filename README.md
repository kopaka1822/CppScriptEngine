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
The easiest way to add custom objects is to derive directly from `ScriptObject`.

### Example
This example explains how to write a simple Vector 2D class. In the end, you should be able to create a new instance of a `Vec2` and know how to register the member functions and use them with the script engine.

The header file for our `Vec2` class looks like this:

```c++
#include "script/objects/GetValueObject.h"

class Vec2 : public script::ScriptObject {
public:
   Vec2(float x, float y);
   // ScriptObject overrides
   std::string toString() const override;
   script::ScriptObjectPtr clone() const override;
   bool equals(const script::ScriptObjectPtr& other) const override;
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
### New Functionality
The new functions can be implemented as usual. The `add(...)` function for example would look like this:
```c++
void Vec2::add(const Vec2& other) {
   x += other.x;
   y += other.y;
}
```
### Registering Functions for Invoke
In order to use the function within the script engine the functions have to be registered with `addFunction(...)` in our constructor. `addFunction` takes two arguments: The first is a string that defines under which name the function will be available. The second one has the type `ScriptObject::FunctionT` which is defined as `std::function<ScriptObjectPtr(const ScriptPtr<ArrayObject>&)>`. That means we need a function that takes an `ArrayObject` (which is an array of ScriptObjects) and returns a `ScriptObject`. 

Our first attempt to register the `add(...)` function might look like this:
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
However, this snippet contains a lot of argument verification to ensure that the correct arguments were passed to the function. Fortunately, the script engine is equipped with an `Util` class that contains a lot of helpful functions that already do argument verification.

The `Util` version looks like this:
```c++
addFunction("add", script::Util::makeFunction(this, &Vec2::add, "add(Vec2)"));
```
This small piece of code executed exactly the same piece of code as above.

The remaining functions will be registered similarly and our constructor looks like this:
```c++
Vec2::Vec2(float x, float y) : x(x), y(y) {
   addFunction("add", script::Util::makeFunction(this, &Vec2::add, "Vec2::add(Vec2)"));
   addFunction("subtract", script::Util::makeFunction(this, &Vec2::subtract, "Vec2::subtract(Vec2)"));
   addFunction("negate", script::Util::makeFunction(this, &Vec2::negate, "Vec2::negate()"));
   addFunction("getX", script::Util::makeFunction(this, &Vec2::getX, "Vec2::float getX()"));
   addFunction("getY", script::Util::makeFunction(this, &Vec2::getY, "Vec2::float getY()"));
   addFunction("setX", script::Util::makeFunction(this, &Vec2::setX, "Vec2::setX(float x)"));
   addFunction("setY", script::Util::makeFunction(this, &Vec2::setY, "Vec2::setY(float y)"));
}
```

### Adding the Constructor
To create a new instance of our `Vec2` class we need to register a constuctor as a static function. First we implement the static `FunctionT Vec2::getCtor()` function:
```c++
script::ScriptObject::FunctionT Vec2::getCtor() {
   return script::Util::fromLambda([](float x, float y) {
      return std::make_shared<Vec2>(x, y);
   }, "Vec2(float x, float y)");
}
```
Since there is no easy way to call the contructor with the `Util` functions we need to create a lambda function that creates an instance for us. Last, we need to register this function in our script engine:
```c++
script::ScriptEngine engine;
engine.setStaticFunction("Vec2", Vec2::getCtor());
...
```
Now, we can use our `Vec2` class within the engine:
```
a = Vec2(1.0f, 0.0f)
>> Vec2(1.000000, 0.000000)
a.add(Vec2(9.0f, -10.0f))
>> Vec2(10.000000, -10.000000)
```

## Embed object with ValueObject

If you have already written a class and you don't want to change it's inheritance you can embed the object within the `script::ValueObject<T>`. The `ValueObject<T>` class has an internal instance of the object (`T`) that can be accessed through the protected member variable `m_value`. `ValueObject<T>` inherits from `GetValueObject<T>` which exposes a public `virtual T& getValue()` function to access the stored value. This function is used within the `Util` class to convert arguments. That means, if a function expected an `int`, `Util` tries to convert the argument to a `GetValueObject<int>` to retrieve the `int` value.

The following example shows how to embed the `std::fstream` class into a `script::ValueObject<T>`:
```
#include <fstream>
#include <script/ScriptEngine.h>

class FileObject : public script::ValueObject<std::fstream> {
public:
   FileObject(const std::string& filename);
   static FunctionT getCtor();
};
```
Of course we can use some functions from the `Util` class as well to register out functions for invoke. The full documentation for all `Util` functions can be found HERE TODO.
```c++
FileObject::FileObject(const std::string& filename) :
   ValueObject<std::fstream>(filename.c_str(), std::fstream::out) {
   
   // simple function with known return value
   addFunction("isOpen", script::Util::makeFunction(&m_value, &std::fstream::is_open, "bool FileObject::isOpen()"));
   // function that should return a reference to this object (needs this pointer)
   addFunction("close", script::Util::makeFunction(this, &m_value, &std::fstream::close, "FileObject::close()"));
   // function that was implemented in a derived class and should return this object
   addFunction("flush", script::Util::makeFunction(this, static_cast<std::ostream*>(&m_value), &std::fstream::flush, "FileObject::flush()"));
   // function that works with every object
   addFunction("write", [this](const script::ArrayObjectPtr& args) {
      for (const auto& obj : *args) {
         // write string representation into the stream
         m_value << script::Util::getBareString(obj);
      }
      return this->shared_from_this();
   });
}
```

# Naming Conventions
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
