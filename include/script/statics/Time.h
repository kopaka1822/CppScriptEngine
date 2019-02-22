#pragma once
#include <chrono>

namespace script::time
{
	using Timestamp = std::chrono::high_resolution_clock::time_point;
	using Duration = std::chrono::high_resolution_clock::duration;
}