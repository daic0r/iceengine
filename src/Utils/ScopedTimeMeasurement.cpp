#include <Utils/ScopedTimeMeasurement.h>

namespace Ice {

ScopedTimeMeasurement::ScopedTimeMeasurement(OnCompletionFuncType&& func)
	: m_onCompletionFunc{ std::move(func) } {}

ScopedTimeMeasurement::ScopedTimeMeasurement(const OnCompletionFuncType& func)
	: m_onCompletionFunc{ func } {}

ScopedTimeMeasurement::~ScopedTimeMeasurement() {
	if (m_onCompletionFunc) {
		auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_initTime);
		m_onCompletionFunc(dur);
	}
}

}