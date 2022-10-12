//
//  TextLabel.h
//  IceCraft
//
//  Created by Matthias Gruen on 19.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef TextLabel_h
#define TextLabel_h

#include <IceEngine.h>

#include <string>
#include <vector>

#include <GUI/Widget.h>
#include <GUI/Fonts/TextStyle.h>
#include <GUI/Fonts/Word.h>

namespace Ice {

struct FontInfo;
class IWidgetRenderer;

namespace GUI {

class TextLabel : public Widget {
    friend class TextRendererGL;
	friend class WidgetManager;

	// For use by the widget manager
	static constexpr bool MultilineDefaultSetting = true;
    
    TextStyle m_style;
    HorizontalAlignment m_horizTextAlignment{ HorizontalAlignment::LEFT };
    VerticalAlignment m_vertTextAlignment{ VerticalAlignment::TOP };
    
    std::vector<Word> m_vWords;
    std::vector<float> m_vVertexBuffer;
    float m_fHeight{ 0.0f };
    bool m_bMultiline = MultilineDefaultSetting;
    std::string m_strText;

public:
    TextLabel();
    TextLabel(const std::string& strText, bool bMultiline, const TextStyle& style);
    
    void setForegroundColor(const glm::vec4& color) noexcept override { m_style.color() = color; }
    const glm::vec4& foregroundColor() const noexcept override { return m_style.color(); }

    const auto& text() const noexcept { return m_strText; }
    void setText(const std::string& strText) noexcept { m_strText = strText; setGraphicsDirty(true); }
    
    const TextStyle& style() const noexcept { return m_style; }
    void setStyle(const TextStyle& style) noexcept { m_style = style; setGraphicsDirty(true); }

    auto multiline() const noexcept { return m_bMultiline; }
    void setMultiline(bool b) noexcept { m_bMultiline = b; }

    const auto& words() const noexcept { return m_vWords; }

    auto horizontalTextAlignment() const noexcept { return m_horizTextAlignment; }
    void setHorizontalTextAlignment(HorizontalAlignment h) noexcept { m_horizTextAlignment = h; setGraphicsDirty(true); }
    auto verticalTextAlignment() const noexcept { return m_vertTextAlignment; }
    void setVerticalTextAlignment(VerticalAlignment v) noexcept { m_vertTextAlignment = v; setGraphicsDirty(true); }
    
    const auto& vertexBuffer() const noexcept { return m_vVertexBuffer; }
    
    void render(IWidgetRenderer*);

    void onAddedToParent() override;

protected:
    auto& text() noexcept { return m_strText; }
    void init();
    virtual void generate();

};


}

}


#endif /* TextLabel_h */
