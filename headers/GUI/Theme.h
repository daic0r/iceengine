#ifndef Theme_h
#define Theme_h

#include <string>
#include <glm/vec4.hpp>
#include <GUI/Fonts/TextStyle.h>

namespace Ice {

namespace GUI {

class Theme {
	TextStyle m_textStyle;
	glm::vec4 m_foregroundColor;
	glm::vec4 m_color;

public:
	Theme() noexcept {
		textStyle().color() = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
		textStyle().size() = 18;
		textStyle().fontName() = "Segoe UI";
		textStyle().thickness() = .44f;
		textStyle().edgeWidth() = .15f;
		textStyle().borderWidth() = 0; //.6f;
		textStyle().borderEdge() = 0; //.05f;
		textStyle().shadowOffset() = glm::vec2{ .000f, .000f };
		foregroundColor() = glm::vec4{ .086f, .518f, .674f, 1.0f };
		color() = glm::vec4{ .118f, .118f, .118f, .6f };
	}

	const TextStyle& textStyle() const noexcept { return m_textStyle; }
	TextStyle& textStyle() noexcept { return m_textStyle; }

	const glm::vec4& foregroundColor() const noexcept { return m_foregroundColor; }
	glm::vec4& foregroundColor() noexcept { return m_foregroundColor; }

	const glm::vec4 color() const noexcept { return m_color; }
	glm::vec4& color() noexcept { return m_color; }
};

}

}


#endif
