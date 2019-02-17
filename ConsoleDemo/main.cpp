#include <iostream>
#include <script/ScriptEngine.h>
#include <script/statics/ConsoleStaticObject.h>

int main()
{
	script::ScriptEngine engine;
	engine.setStaticObject("Console", std::make_shared<script::ConsoleStaticObject>());

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
