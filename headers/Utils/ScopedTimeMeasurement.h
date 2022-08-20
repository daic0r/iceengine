#pragma once

#include <chrono>
#include <functional>

namespace Ice {

class ScopedTimeMeasurement {
	using OnCompletionFuncType = std::function<void(std::chrono::nanoseconds)>;
	std::chrono::time_point<std::chrono::steady_clock> m_initTime = std::chrono::steady_clock::now();
	OnCompletionFuncType m_onCompletionFunc;

public:
	ScopedTimeMeasurement() = delete;
	ScopedTimeMeasurement(const ScopedTimeMeasurement&) = delete;
	ScopedTimeMeasurement& operator=(const ScopedTimeMeasurement&) = delete;
	ScopedTimeMeasurement(ScopedTimeMeasurement&&) = delete;
	ScopedTimeMeasurement& operator=(ScopedTimeMeasurement&&) = delete;
	ScopedTimeMeasurement(OnCompletionFuncType&& func);
	ScopedTimeMeasurement(const OnCompletionFuncType& func);
	~ScopedTimeMeasurement();
};

}