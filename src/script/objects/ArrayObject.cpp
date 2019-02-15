#include "../../../include/script/objects/ArrayObject.h"
#include "../../../include/script/Util.h"
#include <cassert>
#include <mutex>

script::ArrayObject::ArrayObject()
	:
ArrayObject(std::vector<ScriptObjectPtr>{})
{}

script::ArrayObject::ArrayObject(std::vector<ScriptObjectPtr> values)
	:
	m_values(std::move(values))
{
	addFunction("get", Util::makeFunction(this, &ArrayObject::get, "ArrayObject::get(int index)"));
	addFunction("add", [this](const ArrayObjectPtr& args)
	{
		// add all elements
		this->addAll(args);
		return this->shared_from_this();
	});
	addFunction("addAll", Util::makeFunction(this, &ArrayObject::addAll, "ArrayObject::addAll(ArrayObject)"));
	addFunction("remove", Util::makeFunction(this, &ArrayObject::remove, "ArrayObject::remove(int index)"));
	addFunction("clear", Util::makeFunction(this, &ArrayObject::clear, "ArrayObject::clear()"));
	addFunction("count", Util::makeFunction(this, &ArrayObject::count, "int ArrayObject::count()"));
	addFunction("slice", Util::makeFunction(this, &ArrayObject::slice, "ArrayObject ArrayObject::slice(int from, int count)"));
}

std::string script::ArrayObject::toString() const
{
	if (m_values.empty())
		return "[]";

	if (m_toStringMutex.locked())
		return "[...]";

	std::lock_guard<BoolMutex> g(m_toStringMutex);

	std::string res = "[";
	for (size_t i = 0; i < m_values.size() - 1; ++i)
	{
		res += m_values[i]->toString();
		res += ", ";
	}
	res += m_values.back()->toString();
	res += "]";
	return res;
}

const script::ScriptObjectPtr& script::ArrayObject::get(int index) const
{
	if (index >= int(m_values.size()) || index < 0)
		throw std::out_of_range("ArrayObject::get index out of range: " + std::to_string(index));

	return m_values[index];
}

void script::ArrayObject::add(ScriptObjectPtr object)
{
	assert(object != nullptr);
	//if (!object)
	//	throw std::runtime_error("ArrayObject::add object not set to a reference (nullptr)");

	m_values.push_back(std::move(object));
}

void script::ArrayObject::remove(int index)
{
	if (index >= int(m_values.size()) || index < 0)
		throw std::out_of_range("ArrayObject::remove index out of range: " + std::to_string(index));

	m_values.erase(m_values.begin() + index);
}

void script::ArrayObject::clear()
{
	m_values.clear();
}

void script::ArrayObject::addAll(const ScriptPtr<ArrayObject>& other)
{
	// add all elements
	const int count = other->count();
	for (int i = 0; i < count; ++i)
		m_values.push_back(other->get(i));
}

std::shared_ptr<script::ArrayObject> script::ArrayObject::slice(int from, int count)
{
	if (from >= int(m_values.size()) || from < 0)
		throw std::out_of_range("ArrayObject::slice from out of range");
	if (count < 0)
		throw std::runtime_error("ArrayObject::slice count may not be smaller than zero");
	if (from + count > int(m_values.size()))
		throw std::out_of_range("ArrayObject::slice count exceeds array");

	std::vector<ScriptObjectPtr> res;
	for (int i = from; i < from + count; ++i)
		res.push_back(m_values[i]);

	return std::make_shared<ArrayObject>(res);
}

int script::ArrayObject::count() const
{
	return static_cast<int>(m_values.size());
}

script::ScriptObjectPtr script::ArrayObject::clone() const
{
	decltype(m_values) copy;
	copy.reserve(m_values.size());
	for (const auto& v : m_values)
	{
		copy.push_back(v->clone());
	}
	return std::make_shared<ArrayObject>(copy);
}

bool script::ArrayObject::equals(const ScriptObjectPtr& other) const
{
	// reference equals
	if (this == other.get()) return true;

	// other must be an array
	const auto arr = dynamic_cast<const ArrayObject*>(other.get());
	if (arr == nullptr) return false;

	if (count() != arr->count())
		return false;

	// memberwise compare
	for (int i = 0; i < count(); ++i)
		if (!m_values[i]->equals(arr->m_values[i]))
			return false;

	return true;
}
