#pragma once
#include <locale>

namespace script
{
	// allows alpha numeric characters and the underscore
	inline bool isIdentifier(char c)
	{
		return isalnum(static_cast<unsigned char>(c)) || c == '_';
	}
}
