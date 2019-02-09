#include "../../../include/script/objects/NullObject.h"
#include "../../../include/script/Util.h"

std::string script::NullObject::toString() const
{
	return "null";
}

script::ScriptObjectPtr script::NullObject::clone() const
{
	return get();
}

script::ScriptObjectPtr script::NullObject::get()
{
	static auto instance = std::shared_ptr<ScriptObject>(new NullObject());
	return instance;
}