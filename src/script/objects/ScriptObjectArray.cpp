#include "../../../include/script/objects/ScriptObjectArray.h"
#include "../../../include/script/Util.h"

script::ScriptObjectArray::ScriptObjectArray(std::vector<ScriptObjectPtr> values)
	:
	m_values(std::move(values))
{
	addFunction("get", Util::makeFunction(this, &ScriptObjectArray::get, "ScriptObjectArray::get(int index)"));
	addFunction("add", [this](const ScriptObjectArrayPtr& args)
	{
		// add all elements
		this->addAll(args);
		return this->shared_from_this();
	});
	addFunction("addAll", Util::makeFunction(this, &ScriptObjectArray::addAll, "ScriptObjectArray::addAll(ScriptObjectArray)"));
	addFunction("remove", Util::makeFunction(this, &ScriptObjectArray::remove, "ScriptObjectArray::remove(int index)"));
	addFunction("clear", Util::makeFunction(this, &ScriptObjectArray::clear, "ScriptObjectArray::clear()"));
	addFunction("count", Util::makeFunction(this, &ScriptObjectArray::count, "int ScriptObjectArray::count()"));
	addFunction("slice", Util::makeFunction(this, &ScriptObjectArray::slice, "ScriptObjectArray ScriptObjectArray::slice(int from, int count)"));
}

std::string script::ScriptObjectArray::toString() const
{
	if (m_values.empty())
		return "[]";

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

const script::ScriptObjectPtr& script::ScriptObjectArray::get(int index) const
{
	if (index >= int(m_values.size()) || index < 0)
		throw std::out_of_range("ScriptObjectArray::get index out of range: " + std::to_string(index));

	return m_values[index];
}

void script::ScriptObjectArray::add(ScriptObjectPtr object)
{
	if (!object)
		throw std::runtime_error("ScriptObjectArray::add object not set to a reference (nullptr)");
	m_values.push_back(std::move(object));
}

void script::ScriptObjectArray::remove(int index)
{
	if (index >= int(m_values.size()) || index < 0)
		throw std::out_of_range("ScriptObjectArray::remove index out of range: " + std::to_string(index));

	m_values.erase(m_values.begin() + index);
}

void script::ScriptObjectArray::clear()
{
	m_values.clear();
}

void script::ScriptObjectArray::addAll(const ScriptPtr<ScriptObjectArray>& other)
{
	// add all elements
	const int count = other->count();
	for (int i = 0; i < count; ++i)
		m_values.push_back(other->get(i));
}

std::shared_ptr<script::ScriptObjectArray> script::ScriptObjectArray::slice(int from, int count)
{
	if (from >= int(m_values.size()) || from < 0)
		throw std::out_of_range("ScriptObjectArray::slice from out of range");
	if (count < 0)
		throw std::runtime_error("ScriptObjectArray::slice count may not be smaller than zero");
	if (from + count > int(m_values.size()))
		throw std::runtime_error("ScriptObjectArray::slice count exceeds array");

	std::vector<ScriptObjectPtr> res;
	for (int i = from; i < from + count; ++i)
		res.push_back(m_values[i]);

	return std::make_shared<ScriptObjectArray>(res);
}

int script::ScriptObjectArray::count() const
{
	return static_cast<int>(m_values.size());
}

script::ScriptObjectPtr script::ScriptObjectArray::clone() const
{
	decltype(m_values) copy;
	copy.reserve(m_values.size());
	for (const auto& v : m_values)
	{
		copy.push_back(v->clone());
	}
	return std::make_shared<ScriptObjectArray>(copy);
}
