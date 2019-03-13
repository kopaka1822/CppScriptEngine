#pragma once
#include "ValueComparableObject.h"
#include "../Util.h"

namespace script
{
	template<class T>
	class EnumObject final : public ValueComparableObject<T>
	{
	public:
		EnumObject(T value, std::string text)
			:
		ValueComparableObject<T>(value),
		m_text(move(text))
		{}
		~EnumObject() override = default;

		std::string toString() const override
		{
			return m_text;
		}

		ScriptObjectPtr clone() const override
		{
			return std::make_shared<EnumObject<T>>(this->m_value, this->m_text);
		}
	private:
		std::string m_text;
	};

	template<class T>
	class StaticEnumObject : public ScriptObject
	{
	public:
		std::shared_ptr<EnumObject<T>> parse(const std::string& text)
		{
			const auto it = m_map.find(text);
			if (it == m_map.end())
				throw std::runtime_error(this->toString() + "::parse cannot convert \"" + text + "\" to enum");

			return std::make_shared<EnumObject<T>>(it->second, Util::prettyTypeName(typeid(T).name()) + "::" + it->first);
		}

		std::vector<std::string> getValues() const
		{
			std::vector<std::string> res;
			res.reserve(m_map.size());
			for (const auto& v : m_map)
				res.push_back(v.first);

			return res;
		}
	protected:
		StaticEnumObject()
		{
			this->addFunction("parse", Util::makeFunction(this, &StaticEnumObject<T>::parse, "enum StaticEnumObject::parse(string)"));
			this->addFunction("getValues", Util::makeFunction(this, &StaticEnumObject<T>::getValues, "array StaticEnumObject::getValues()"));
		}

		void addValue(T value, const std::string& text)
		{
			auto funcName = "get" + text;

			if (text.empty() || !isupper(text.front()))
				throw std::runtime_error("StaticEnumObject::addValue parameter text must start with an uppercase letter");

			if (m_map.find(text) != m_map.end())
				throw std::runtime_error("StaticEnumObject::addValue \"" + text + "\" was already added");

			m_map[text] = value;

			this->addFunction(funcName, Util::fromLambda([value, name = Util::prettyTypeName(typeid(T).name()) + "::" + text]()
			{
				return std::make_shared<EnumObject<T>>(value, name);
			}, this->toString() + "::" + funcName + "()"));
		}

	private:
		std::unordered_map<std::string, T> m_map;
	};
}