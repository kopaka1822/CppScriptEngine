#pragma once
#include <fstream>
#include <script/ScriptEngine.h>

class FileObject : public script::ValueObject<std::fstream>
{
public:
	FileObject(const std::string& filename);
	~FileObject() override = default;

	static FunctionT getCtor();
};