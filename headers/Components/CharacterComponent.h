#pragma once

#include <Components/ComponentIds.h>
#include <deque>

namespace Ice {

struct CharacterComponent {
	static constexpr EntityComponentId id() noexcept { return CHARACTER_COMPONENT; }

	enum class Action {
		NONE,
		WALK,
		PLACE_BIOME_NODE
	};

	std::string m_strName;
	std::uint8_t m_nHealth{ 100 };
	std::deque<Action> actions;
};

}