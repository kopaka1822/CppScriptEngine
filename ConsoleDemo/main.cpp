#include <iostream>
#include <script/ScriptEngine.h>
#include <script/statics/ConsoleStaticObject.h>
#include "Vec2.h"
#include "FileObject.h"

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
			std::cout << ">> " << engine.execute(command) << std::endl;
		}
		catch (const std::exception& e)
		{
			std::cerr << "ERR: " << e.what() << std::endl;
		}
	}
	return 0;
}
