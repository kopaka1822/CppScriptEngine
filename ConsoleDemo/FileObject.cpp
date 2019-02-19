#include "FileObject.h"

FileObject::FileObject(const std::string& filename)
	:
ValueObject<std::fstream>(filename.c_str())
{
	// simple function with known return value
	addFunction("isOpen", script::Util::makeFunction(&m_value, &std::fstream::is_open, "bool FileObject::isOpen()"));
	// function that should return a reference to this object (needs this pointer)
	addFunction("close", script::Util::makeFunction(this, &m_value, &std::fstream::close, "FileObject::close()"));
	// function that was implemented in a derived class and should return this object
	addFunction("flush", script::Util::makeFunction(this, static_cast<std::ostream*>(&m_value), &std::fstream::flush, "FileObject::flush()"));
	// function that works with every object
	addFunction("write", [this](const script::ArrayObjectPtr& args)
	{
		for (const auto& obj : *args)
		{
			// write string representation into the stream
			m_value << script::Util::getBareString(obj);
		}
		return this->shared_from_this();
	});
}
