#include <script/ScriptEngine.h>
#include <iostream>
#include "script/Util.h"
#include "script/Exception.h"

class Renderer
{
public:
	void render(int& iterations)
	{
		std::cout << iterations << std::endl;
		iterations++;
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


	auto str1 = script::Util::makeObject(std::string("test"));
	auto str2 = script::Util::makeObject(std::string("test"));
	auto int1 = script::Util::makeObject(5);
	auto int2 = script::Util::makeObject(6);
	//auto null = script::NullObject::get();
	auto null = script::Util::makeObject(nullptr);
	auto arr = script::Util::makeArray(nullptr, str2);

	//auto cloned = str1->invoke("clone", arr);
	auto res = str1->invoke("equals", arr);
	std::cout << res->toString();
	auto renderer = std::make_shared<RendererWrapper>();

	//renderer->invoke("render", arr);
	//std::cout << int1->toString();

	return 0;
}
catch(const std::exception& e)
{
	std::cerr << e.what();
	return -1;
}
