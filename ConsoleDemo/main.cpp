#include <iostream>
#include <script/ScriptEngine.h>
#include "Vec2.h"
#include "FileObject.h"
#include <chrono>

int main()
{
	script::ScriptEngine engine;
	engine.setStaticFunction("Vec2", Vec2::getCtor());
	engine.setStaticFunction("File", FileObject::getCtor());

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
