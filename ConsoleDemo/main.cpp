#include <iostream>
#include <script/ScriptEngine.h>

int main()
{
	script::ScriptEngine engine;

	std::string command;
	while (true)
	{
		std::getline(std::cin, command);
		engine.execute(command);
	}
}
