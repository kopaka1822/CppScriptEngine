#include <script/ScriptEngine.h>
#include <iostream>

int main() try
{
	auto str1 = std::make_shared<script::StringObject>("hallo");
	auto str2 = std::make_shared<script::StringObject>(" welt");
	auto int1 = std::make_shared<script::IntObject>(6);

	auto emtpy = std::make_shared<script::ScriptObjectArray>();
	auto arr = std::make_shared<script::ScriptObjectArray>();
	//arr->add(str2);
	//arr->add(str2);
	arr->add(int1);

	auto res = str1->invoke("clone", emtpy)->invoke("add", arr);

	std::cout << str1->toString() << std::endl;
	std::cout << res->toString() << std::endl;

	return 0;
}
catch(const std::exception& e)
{
	std::cerr << e.what();
	return -1;
}