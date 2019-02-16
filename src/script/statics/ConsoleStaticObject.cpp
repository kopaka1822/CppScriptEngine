#include "../../../include/script/statics/ConsoleStaticObject.h"
#include "../../../include/script/objects/ArrayObject.h"
#include "../../../include/script/objects/StringObject.h"
#include <iostream>
#include "script/Util.h"

script::ConsoleStaticObject::ConsoleStaticObject()
{
	auto writeFunc = [this](const ArrayObjectPtr& args)
	{
		for (int i = 0; i < args->getCount(); ++i)
		{
			auto strObj = std::dynamic_pointer_cast<StringObject>(args->get(i));
			if (strObj)
				write(strObj->getValue());
			else
				write(args->get(i)->toString());
		}
		return this->shared_from_this();
	};
	addFunction("write", writeFunc);
	addFunction("writeLine", [writeFunc, this](const ArrayObjectPtr& args)
	{
		auto res = writeFunc(args);
		this->newline();
		return res;
	});
	addFunction("newline", Util::makeFunction(this, &ConsoleStaticObject::newline, "ConsoleStaticObject::newline()"));
	addFunction("read", Util::makeFunction(this, &ConsoleStaticObject::read, "string ConsoleStaticObject::read()"));
}

void script::ConsoleStaticObject::write(const std::string& value)
{
	std::cout << value;
}

void script::ConsoleStaticObject::newline()
{
	std::cout << std::endl;
}

std::string script::ConsoleStaticObject::read()
{
	std::string res;
	std::getline(std::cin, res);
	return res;
}
