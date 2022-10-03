#pragma once

#include <Components/ComponentIds.h>

namespace Ice {

struct CharacterComponent {
	static constexpr EntityComponentId id() noexcept { return CHARACTER_COMPONENT; }

	enum class Action {
		NONE,
		WALK,
		PLACE_ITEM
	};

	std::string m_strName;
	std::uint8_t m_nHealth{ 100 };
	Action currentAction{};
};

}