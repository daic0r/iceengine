#pragma once

#include <Entities/EntityComponentSystem.h>
#include <Components/SunComponent.h>
#include <chrono>
#include <date/date.h>

namespace Ice {

class DayNightCycleSystem : public EntityComponentSystem<SunComponent> {
	using Clock = std::chrono::system_clock;
	using dbl_dur = std::chrono::duration<double>;

	time_t m_currentTime;

	float getDayOfYear(std::chrono::time_point<std::chrono::system_clock>&) const noexcept;
 	float getRotationAxisTiltAngle(std::chrono::time_point<std::chrono::system_clock>&) const noexcept;
	float getDayProgression(const std::chrono::time_point<std::chrono::system_clock>&) const noexcept;
	glm::vec3 getSunPosition(float& fOutDayProgression) const noexcept;

	static constexpr auto getSunDistance() noexcept {
		return 100000.0f;
	}
	static constexpr auto getSunrise() noexcept {
		using namespace std::chrono;
		auto ret = date::hh_mm_ss{ 6.h  /*98min + 2020s + 0.5s*/ };    // C++17
		return ret;
	}
	static constexpr auto getSunset() noexcept {
		using namespace std::chrono;
		auto ret = date::hh_mm_ss{ 22.h  /*98min + 2020s + 0.5s*/ };    // C++17
		return ret;
	}
	


public:
	bool update(float fDeltaTime) noexcept override;
	const SunComponent& sun() const;

};

}