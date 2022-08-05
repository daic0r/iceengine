#pragma once

#include <Components/ComponentIds.h>

namespace Ice {

struct CharacterComponent {
	static constexpr EntityComponentId id() noexcept { return CHARACTER_COMPONENT; }

	std::string m_strName;
	std::uint8_t m_nHealth{ 100 };
};

}