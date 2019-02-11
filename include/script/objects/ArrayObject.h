#pragma once
#include <vector>
#include "ScriptObject.h"

namespace script
{
	class ArrayObject final : public ScriptObject
	{
	public:
		ArrayObject() = default;
		explicit ArrayObject(std::vector<ScriptObjectPtr> values);

		std::string toString() const override final;
		ScriptObjectPtr clone() const override final;

		const ScriptObjectPtr& get(int index) const;
		void add(ScriptObjectPtr object);
		void remove(int index);
		void clear();
		void addAll(const ScriptPtr<ArrayObject>& other);
		int count() const;
		/// \brief returns the array subset [from, from + count) as shallow copy
		ScriptPtr<ArrayObject> slice(int from, int count);
	private:
		std::vector<ScriptObjectPtr> m_values;
	};

	using ArrayObjectPtr = ScriptPtr<ArrayObject>;
}