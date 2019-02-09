#include <script/ScriptEngine.h>
#include <iostream>
#include "script/Util.h"
#include "script/Exception.h"

class Renderer
{
public:
	void render(const int* iterations)
	{
		if (iterations == nullptr)
			std::cout << "infinity!";
		else 
			std::cout << *iterations;
	}
};

class RendererWrapper : public script::ValueObject<Renderer>
{
public:
	RendererWrapper()
	{
		addFunction("render", script::Util::makeFunction(this, &m_value, &Renderer::render, "render(int* iterations)"));
	}

	std::string toString() const override
	{
		return "RendererWrapper";
	}

	script::ScriptObjectPtr clone() const override
	{
		throw script::ObjectNotCloneableException("RendererWrapper");
	}
};

/*template<class T>
T& test(int& a)
{
	return a;
}

template<class T>
T* test(int& a)
{
	return &a;
}*/

int main() try
{


	auto str1 = script::Util::toScriptObject(std::string("test"));
	auto str2 = script::Util::toScriptObject(std::string("test"));
	auto int1 = script::Util::toScriptObject(5);
	auto int2 = script::Util::toScriptObject(6);
	auto null = script::NullObject::get();
	auto arr = script::Util::makeArray(null);

	auto renderer = std::make_shared<RendererWrapper>();

	renderer->invoke("render", arr);

	return 0;
}
catch(const std::exception& e)
{
	std::cerr << e.what();
	return -1;
}
