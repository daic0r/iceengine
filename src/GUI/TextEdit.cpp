#include <GUI/TextEdit.h>

#include <SDL2/SDL.h>
#include <System/SystemServices.h>
#include <GUI/WidgetManager.h>
#include <GUI/Constraints/DynamicConstraint.h>
#include <GUI/Constraints/TwoPointSizeConstraint.h>
#include <GUI/Constraints/AlignmentConstraint.h>
#include <GUI/Constraints/DirectSizeConstraint.h>
#include <GUI/Fonts/FontInfo.h>
#include <Interfaces/IGraphicsSystem.h>

namespace Ice {

namespace GUI {

TextEdit::TextEdit() {
	init();
}

TextEdit::TextEdit(const std::string& strText, bool bMultiline, const TextStyle& style)
	: TextLabel{ strText, bMultiline, style }
{
	init();
}

void TextEdit::init() noexcept {
	setIsTabStoppable(true);
	setPadding(.0f, .0f, .0f, .0f);
	setMultiline(false);
	setColor(glm::vec4{ .2f, .2f, .2f, 1.0f });
	setVerticalTextAlignment(VerticalAlignment::MIDDLE);

	setOnMouseButtonFunc([](Widget* w, MouseButton b, bool bPressed, bool& bHandled) {
		TextEdit* pTextEdit = static_cast<TextEdit*>(w);
		if ((b == MouseButton::LEFT) && !bPressed && !pTextEdit->active())
			pTextEdit->activate();
	});

	setOnFocusReceivedFunc([](Widget* w) {
		TextEdit* pTextEdit = static_cast<TextEdit*>(w);
		pTextEdit->activate();
		pTextEdit->m_strOldText = pTextEdit->text();
	});

	setOnFocusLostFunc([](Widget* w) {
		TextEdit* pTextEdit = static_cast<TextEdit*>(w);
		pTextEdit->deactivate();
	});

	setOnTextInputFunc([](Widget* w, const sTextInputInfo& ti) {
		TextEdit* pTextEdit = static_cast<TextEdit*>(w);
		if (!pTextEdit->active())
			return;

		if (ti.bBackspace) {
			if (pTextEdit->text().length() > 0)
				pTextEdit->text().pop_back();
		}
		else if (ti.bEnter) {
			pTextEdit->deactivate();
		}
		else if (ti.bEscape) {
			pTextEdit->setText(pTextEdit->m_strOldText);
			pTextEdit->deactivate();
		}
		else if (pTextEdit->text().length() < pTextEdit->maxChars()) {
			pTextEdit->text() += ti.ch;
		}
		pTextEdit->setGraphicsDirty(true);
	});

	setClipFrameFunc([](const Widget* w) {
        return w->extents();
    });

	auto& wm = systemServices.getWidgetManager();
	m_pCursor = wm.createWidget<Widget, true>(this);
	m_pCursor->setPointConstraint(HorizontalAlignment::LEFT, std::make_unique<DynamicPointConstraint>([](Widget* w) {
		auto pTextEdit = static_cast<TextEdit*>(w);
		const auto& words = pTextEdit->words();
        const auto fPixelWidth = pTextEdit->style().getScaledPixelWidth();
        const auto fSpaceWidth = fPixelWidth * pTextEdit->style().fontInfo()->characters.at(32).xadvance;
		float fWidth{ 0.0f };
		for (const auto& word : words) {
			fWidth += word.width();
            // If not the last word, add a space behind it
			if (&word != &words.back())
				fWidth += fSpaceWidth;
		}
		switch (pTextEdit->horizontalTextAlignment()) {
			case HorizontalAlignment::LEFT:
				return pTextEdit->left() + fWidth;
			case HorizontalAlignment::CENTER:
				return (pTextEdit->right() + pTextEdit->left()) / 2.0f + (fWidth / 2.0f);
			case HorizontalAlignment::RIGHT:
				return pTextEdit->right();
		}
		throw std::runtime_error("Unreachable");
	}));
	m_pCursor->setPointConstraint(VerticalAlignment::MIDDLE, std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::MIDDLE));
	m_pCursor->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DirectSizeConstraint<Pixels>>(2_px));
	const auto fHeight = static_cast<float>(style().fontInfo()->lineHeight) * style().getScaledPixelHeight();
	m_pCursor->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<DirectSizeConstraint<NormalizedPixels>>(NormalizedPixels{ fHeight }));
	m_pCursor->setColor(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	m_pCursor->setVisible(false);
}

void TextEdit::activate() noexcept {
	SDL_StartTextInput();
	m_bActive = true;
	animations().emplace_back([](Widget* w, float fDeltaTime) {
		TextEdit* pTextEdit = static_cast<TextEdit*>(w);
		if (!pTextEdit->active())
			return true;

		pTextEdit->m_fTimeElapsed += fDeltaTime;
		if (pTextEdit->m_fTimeElapsed > .5f) {
			pTextEdit->toggleCursor();
			pTextEdit->m_fTimeElapsed = 0.0f;
		}

		return false;
	});
}

void TextEdit::deactivate() noexcept {
	SDL_StopTextInput();
	m_bActive = false;
	if (m_pCursor->visible())
		toggleCursor();
}

void TextEdit::generate() {
	TextLabel::generate();
	m_pCursor->posX(true);
}

}

}
