#include "../../../include/script/statics/SystemObject.h"

script::SystemObject::SystemObject()
{
	addFunction("silent", Util::makeFunction(this, &SystemObject::silent, "null SystemObject::silent()"));
}

script::ScriptObjectPtr script::SystemObject::silent(const ScriptObjectPtr&) const
{
	return NullObject::get();
}
