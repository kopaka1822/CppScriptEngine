#pragma once
#include <vector>
#include "ScriptObject.h"
#include "../BoolMutex.h"

namespace script
{
	class ArrayObject final : public ScriptObject
	{
	public:
		ArrayObject();
		explicit ArrayObject(std::vector<ScriptObjectPtr> values);
		~ArrayObject() override = default;

		std::string toString() const override final;
		ScriptObjectPtr clone() const override final;
		/// \brief memberwise comparision with another array
		bool equals(const ScriptObjectPtr& other) const override;

		const ScriptObjectPtr& get(int index) const;
		void set(int index, ScriptObjectPtr object);
		void add(ScriptObjectPtr object);
		void remove(int index);
		void clear();
		void addAll(const ScriptPtr<ArrayObject>& other);
		int getCount() const;
		/// \brief returns the array subset [from, from + getCount) as shallow copy
		ScriptPtr<ArrayObject> slice(int from, int count);
		/// \brief returns the array subset [from, end) as shallow copy
		ScriptPtr<ArrayObject> slice(int from);

		// c++ helper functions
		std::vector<ScriptObjectPtr>::iterator begin();
		std::vector<ScriptObjectPtr>::iterator end();
		std::vector<ScriptObjectPtr>::const_iterator begin() const;
		std::vector<ScriptObjectPtr>::const_iterator end() const;
	private:
		std::vector<ScriptObjectPtr> m_values;
		mutable BoolMutex m_toStringMutex;
	};

	using ArrayObjectPtr = ScriptPtr<ArrayObject>;
}
