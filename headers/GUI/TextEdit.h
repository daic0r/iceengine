#pragma once

#include <IceEngine.h>

#include "TextLabel.h"

namespace Ice {

namespace GUI {

class ICEENGINE_API TextEdit : public TextLabel {
public:
	TextEdit();
	TextEdit(const std::string& strText, bool bMultiline, const TextStyle& style);

	bool active() const noexcept { return m_bActive; }
	void toggleCursor() noexcept { m_pCursor->setVisible(!m_pCursor->visible()); }

	auto maxChars() const noexcept { return m_nMaxChars; }
	void setMaxChars(size_t n) noexcept { m_nMaxChars = n; }

private:
	bool m_bActive{ false };
	std::string m_strOldText;
	Widget* m_pCursor{ nullptr };
	float m_fTimeElapsed{ 0.0f };
	size_t m_nMaxChars{ 20 };

	void activate() noexcept;
	void deactivate() noexcept;
	void init() noexcept;
	void generate() override;
};


}

}
