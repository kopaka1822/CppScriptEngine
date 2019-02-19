#include <iostream>
#include <script/ScriptEngine.h>
#include <script/statics/ConsoleStaticObject.h>
#include "Vec2.h"

int main()
{
	script::ScriptEngine engine;
	engine.setStaticObject("Console", std::make_shared<script::ConsoleStaticObject>());

	engine.setObject("myVec", std::make_shared<Vec2>(0.0f, 0.0f));

	std::string command;
	while (true)
	{
		std::getline(std::cin, command);
		try
		{
			std::string res;
			engine.execute(command, &res);
			std::cout << ">> " << res << std::endl;
		}
		catch (const std::exception& e)
		{
			std::cerr << "ERR: " << e.what() << std::endl;
		}
	}
	return 0;
}
