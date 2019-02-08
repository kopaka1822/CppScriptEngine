#include "../../../include/script/objects/ScriptObjectArray.h"

script::ScriptObjectArray::ScriptObjectArray(std::vector<ScriptObjectPtr> values)
	:
	m_values(std::move(values))
{
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

const script::ScriptObjectPtr& script::ScriptObjectArray::get(size_t index) const
{
	if (index >= m_values.size())
		throw std::out_of_range("ScriptObjectArray::get index out of range: " + std::to_string(index));

	return m_values[index];
}

void script::ScriptObjectArray::add(ScriptObjectPtr object)
{
	m_values.push_back(std::move(object));
}

size_t script::ScriptObjectArray::count() const
{
	return m_values.size();
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
