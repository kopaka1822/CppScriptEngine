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
		engine.execute(command);
	}
	return 0;
}
