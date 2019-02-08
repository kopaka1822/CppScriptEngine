#pragma once
#include <vector>
#include "ScriptObject.h"

namespace script
{
	class ScriptObjectArray final : public ScriptObject
	{
	public:
		ScriptObjectArray() = default;
		explicit ScriptObjectArray(std::vector<ScriptObjectPtr> values);

		std::string toString() const override final;
		ScriptObjectPtr clone() const override final;

		const ScriptObjectPtr& get(size_t index) const;
		void add(ScriptObjectPtr object);
		size_t count() const;
	private:
		std::vector<ScriptObjectPtr> m_values;
	};

	using ScriptObjectArrayPtr = ScriptPtr<ScriptObjectArray>;
}