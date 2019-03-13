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
		{
			this->addFunction("toInt", Util::makeFunction(this, &EnumObject<T>::toInt, "int EnumObject::toInt()"));
		}
		~EnumObject() override = default;

		/// \brief returns string representation with enum type prefix
		std::string toString() const override
		{
			return m_text;
		}

		/// \brief returns integer representation
		int toInt() const
		{
			return int(this->m_value);
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
		/// \brief tries to convert the text to the enum. Throws if conversion fails
		std::shared_ptr<EnumObject<T>> parse(const std::string& text) const
		{
			const auto it = m_svmap.find(text);
			if (it == m_svmap.end())
				throw std::runtime_error(this->toString() + "::parse cannot convert text \"" + text + "\" to enum");

			return std::make_shared<EnumObject<T>>(it->second, getEnumString(it->first));
		}

		/// \brief tries to convert the value to the enum. Throws if conversion fails
		std::shared_ptr<EnumObject<T>> parse(int value) const
		{
			const auto it = m_vsmap.find(T(value));
			if (it == m_vsmap.end())
				throw std::runtime_error(this->toString() + "::parse cannot convert value " + std::to_string(value) + " to enum");

			return std::make_shared<EnumObject<T>>(it->first, getEnumString(it->second));
		}

		/// \brief returns all legal string values
		std::vector<std::string> getValues() const
		{
			std::vector<std::string> res;
			res.reserve(m_svmap.size());
			for (const auto& v : m_svmap)
				res.push_back(v.first);

			return res;
		}
	protected:
		StaticEnumObject()
		{
			this->addFunction("parse", Util::combineFunctions({
				Util::makeFunction(this, static_cast<std::shared_ptr<EnumObject<T>>(StaticEnumObject<T>::*)(const std::string&) const>(&StaticEnumObject<T>::parse), "enum StaticEnumObject::parse(string)"),
				Util::makeFunction(this, static_cast<std::shared_ptr<EnumObject<T>>(StaticEnumObject<T>::*)(int) const>(&StaticEnumObject<T>::parse), "enum StaticEnumObject::parse(int)")				
			}));
			this->addFunction("getValues", Util::makeFunction(this, &StaticEnumObject<T>::getValues, "array StaticEnumObject::getValues()"));
		}

		/// \brief adds an enum-string pair to the internal list
		/// \param value enum value
		/// \param text text representation of the enum value
		void addValue(T value, const std::string& text)
		{
			auto funcName = "get" + text;

			if (text.empty() || !isupper(text.front()))
				throw std::runtime_error("StaticEnumObject::addValue parameter text must start with an uppercase letter");

			if (m_svmap.find(text) != m_svmap.end())
				throw std::runtime_error("StaticEnumObject::addValue text \"" + text + "\" was already added");
			if (m_vsmap.find(value) != m_vsmap.end())
				throw std::runtime_error("StaticEnumObject::addValue value " + std::to_string(int(value)) + " was already added");

			m_svmap[text] = value;
			m_vsmap[value] = text;

			this->addFunction(funcName, Util::fromLambda([value, name = getEnumString(text)]()
			{
				return std::make_shared<EnumObject<T>>(value, name);
			}, this->toString() + "::" + funcName + "()"));
		}

	private:
		static std::string getEnumString(const std::string& text)
		{
			return Util::prettyTypeName(typeid(T).name()) + "::" + text;
		}

		// string -> value lookup
		std::unordered_map<std::string, T> m_svmap;
		// value -> string lookup
		std::unordered_map<T, std::string> m_vsmap;
	};
}