#include <Components/Systems/DayNightCycleSystem.h>
#include <Entities/EntityManager.h>
#include <chrono>
#include <date/date.h>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

namespace Ice {

float DayNightCycleSystem::getDayOfYear(std::chrono::time_point<std::chrono::system_clock>& tp) const noexcept {
	//using namespace std::chrono;
	using namespace date;

	auto now = std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(now);
	auto tm = *localtime(&tt);
	auto beginning{ year(tm.tm_year + 1900) / month(1) / day(1) };
	auto dateNow{ year(tm.tm_year + 1900) / month(tm.tm_mon + 1) / day(tm.tm_mday) };
	auto numDays = ((sys_days)dateNow - (sys_days)beginning).count();

	tp = now;

	return static_cast<float>(numDays);
}

float DayNightCycleSystem::getRotationAxisTiltAngle(std::chrono::time_point<std::chrono::system_clock>& tp) const noexcept
{
	float fNormalizedDays = static_cast<float>(getDayOfYear(tp)) / (365.0f / 8.0f);
	fNormalizedDays -= 4.0f;

	// Calculate Gaussian distribution function
	float fGauss = (1.0f / 2.506628274631000502415765284811f) * expf(-.5f * powf(fNormalizedDays, 2.0f));
	constexpr float fGaussAt0 = 0.39894228040143267793994605993438f;
	// Normalize to range [0;1]
	fGauss /= fGaussAt0;

	// Angle between 20 and 70 degrees
	const float fAngle = glm::radians(20.0f + (1.0f - fGauss) * 50.0f);

	return fAngle;
}

float DayNightCycleSystem::getDayProgression(const std::chrono::time_point<std::chrono::system_clock>& now) const noexcept
{
	using namespace std::chrono;
//	using namespace date;
	const auto daylight_duration = duration_cast<minutes>(getSunset().to_duration()- getSunrise().to_duration());
	const auto sinceMidnight = (floor<minutes>(now) - floor<days>(now));
	auto t = date::make_time(sinceMidnight);
	const auto sinceSunrise = duration_cast<minutes>(sinceMidnight - getSunrise().to_duration());

	const float fRet = (float)sinceSunrise.count() / (float)daylight_duration.count();

	return fRet;
}

glm::vec3 DayNightCycleSystem::getSunPosition(float& fOutDayProgression) const noexcept {
	std::chrono::time_point<std::chrono::system_clock> current;

	const float fRotAxisAngle = getRotationAxisTiltAngle(current);
	const glm::vec4 zAxis{ 0.0f, 0.0f, 1.0f, 0.0f };
	const glm::vec3 rotAxis = glm::rotate(glm::mat4{ 1.0f }, fRotAxisAngle, glm::vec3{ -1.0f, 0.0f, 0.0f }) * zAxis;

	const float fDayProgress = getDayProgression(current);
	const glm::vec3 toSun = glm::rotate(glm::mat4{ 1.0f }, fDayProgress * static_cast<float>(M_PI), rotAxis) * glm::vec4{ 1.0f, 0.0f, 0.0f, 0.0f };

	fOutDayProgression = fDayProgress;

	return toSun * getSunDistance();
}

bool DayNightCycleSystem::update(float fDeltaTime) noexcept
{
	const auto& ents = entities(entityManager.currentScene());
	if (!ents.empty()) {
		auto& sunComp = entityManager.getComponent<SunComponent>(*ents.begin());

		float fDayProgress;
		sunComp.m_light.setPosition(getSunPosition(fDayProgress));
		glm::vec3 sunColor{ 1.0f, 0.0f, 0.3f };
		sunColor.g = 1.0f - fabsf(fDayProgress * 2.0f - 1.0f);
		sunComp.m_light.setColor(sunColor);
		sunComp.m_light.setAmbient(glm::vec3{ .3f, .3f, .3f });
	}

	return true;
}

const SunComponent& DayNightCycleSystem::sun() const
{
	const auto& ents = entities(entityManager.currentScene());
	if (ents.size() != 1)
		throw std::runtime_error("We must have exactly one sun!");
	const auto& sunComp = entityManager.getComponent<SunComponent>(*ents.begin());
	return sunComp;
}

}
