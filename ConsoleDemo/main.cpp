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

	std::chrono::high_resolution_clock::time_point time1 = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point time2 = std::chrono::high_resolution_clock::now();

	std::chrono::high_resolution_clock::duration dur = time1 - time2;

	time1 += dur;

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
