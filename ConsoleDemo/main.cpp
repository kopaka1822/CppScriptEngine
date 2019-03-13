#include <iostream>
#include <script/ScriptEngine.h>
#include "Vec2.h"
#include "FileObject.h"
#include <chrono>
#include "MyEnum.h"

int main()
{
	script::ScriptEngine engine;
	engine.setStaticFunction("Vec2", Vec2::getCtor());
	engine.setStaticFunction("File", FileObject::getCtor());
	engine.setStaticObject("MyEnum", std::make_shared<MyEnumStaticObject>());

	// auto completion test function
	engine.setStaticFunction("Autocomp", script::Util::fromLambda([&engine](const std::string& text)
	{
		auto res = engine.getAutocomplete(text);
		for (const auto& v : res)
			std::cout << text << v << '\n';

		return static_cast<int>(res.size());
	}, "Autocomp(string)"));

	std::string command;
	while (true)
	{
		std::getline(std::cin, command);
		try
		{
			std::cout << ">> " << engine.execute(command)->toString() << std::endl;
		}
		catch (const std::exception& e)
		{
			std::cerr << "ERR: " << e.what() << std::endl;
		}
	}
	return 0;
}
