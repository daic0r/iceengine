//
//  TextLabel.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 19.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GUI/TextLabel.h>
#include <System/SystemServices.h>
#include <Interfaces/ITextManager.h>
#include <System/Tools.h>
#include <GUI/Fonts/FontInfo.h>
#include <Interfaces/IWidgetRenderer.h>
#include <GUI/Constraints/DirectSizeConstraint.h>
#include <GUI/Constraints/DynamicConstraint.h>
#include <System/Pixels.h>
#include <Interfaces/IGraphicsSystem.h>
#include <iostream>

namespace Ice {

namespace GUI {

TextLabel::TextLabel() {
    setColor(glm::vec4{ 0.0f, 0.0f, 0.0f, 0.0f });
	setForegroundColor(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
    setPadding(0.0f, 0.0f, 0.0f, 0.0f);
    
    setOnWindowResizedFunc([](Widget* w, int width, int height) {
        w->setGraphicsDirty(true);
    });
    
    setGraphicsDirty(true);
    init();
}

TextLabel::TextLabel(const std::string& strText, bool bMultiline, const TextStyle& style) : TextLabel{} {
    m_style = style;
    m_strText = strText;
	m_bMultiline = bMultiline;
}

void TextLabel::init() {
    
    // Set height constraint automaticlly to fit the text
    // Can be overridden by user after construction if need be
    setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<DynamicSizeConstraint>([](Widget* w) {
        auto pLabel = static_cast<TextLabel*>(w);
        if (pLabel->graphicsDirty())
            pLabel->generate();
        return pLabel->m_fHeight;
    }, true, this));
}

void TextLabel::generate() {
    auto vWords = Tools::tokenize(text());
    if (vWords.size() == 0)
        return;

    m_vVertexBuffer.clear();
    m_vWords.clear();
    float fPosX = 0.0f;
    float fPosY = 0.0f;
    const auto& fontInfo = systemServices.getTextManager()->getFont(m_style.fontName());
    if (style().size() < 0)
        m_style.size() = contentHeight() * systemServices.getGraphicsSystem()->displayHeight() / 2.0f;
    const float fFactor = static_cast<float>(style().size()) / static_cast<float>(fontInfo.size);
    const float fPixelWidth = fFactor * systemServices.getGraphicsSystem()->getHorizontalPixelScale();
    const float fPixelHeight = fFactor * systemServices.getGraphicsSystem()->getVerticalPixelScale();
    const float fSpaceAdvance = fPixelWidth * static_cast<float>(fontInfo.characters.at(32).xadvance);
    
    std::vector<std::pair<size_t, float>> vLineBeginnings;
    vLineBeginnings.emplace_back(std::make_pair(0, -1.0f));;
    float fLineWidth{ 0.0f };
    
    
    auto adjustPosX = [this, &fPosX, &fLineWidth](){
        switch (horizontalTextAlignment()) {
            case HorizontalAlignment::LEFT:
                fPosX = padding().padding(OrthogonalDirection::LEFT);
                switch (horizAlignment()) {
                    case HorizontalAlignment::LEFT:
                        break;
                    case HorizontalAlignment::CENTER:
                        fPosX -= width() / 2.0f;
                        break;
                    case HorizontalAlignment::RIGHT:
                        fPosX -= width();
                        break;
                }
                break;
            case HorizontalAlignment::CENTER:
                fPosX = -(fLineWidth / 2.0f);
                switch (horizAlignment()) {
                    case HorizontalAlignment::LEFT:
                        fPosX += (width() / 2.0f);
                        break;
                    case HorizontalAlignment::CENTER:
                        break;
                    case HorizontalAlignment::RIGHT:
                        fPosX -= (width() / 2.0f);
                        break;
                }
                break;
            case HorizontalAlignment::RIGHT:
                fPosX = -fLineWidth - padding().padding(OrthogonalDirection::RIGHT);
                switch (horizAlignment()) {
                    case HorizontalAlignment::LEFT:
                        fPosX += width();
                        break;
                    case HorizontalAlignment::CENTER:
                        fPosX += (width() / 2.0f);
                        break;
                    case HorizontalAlignment::RIGHT:
                        break;
                }
                break;
            default:
                break;
        }
    };
    
    size_t nIndex{ 0 };
    size_t nCharCount{ 0 };
    // Don't know if the height of the first line is necessary, but leaving it in for now
    int nFirstLineMaxHeight{ 0 };
    // Determine start point on each line depending on the horizontal alignment
    for (const auto& strWord : vWords) {
        auto word = Word{ strWord, &style() };
        if (multiline() && fLineWidth + word.width() > (right() - left())) {
            adjustPosX();
            fLineWidth = 0.0f;
            vLineBeginnings.back().second = fPosX;
            vLineBeginnings.emplace_back(std::make_pair(nIndex, -1.0f)); // line goes from last iterator up to (not including) this one
        }
        fLineWidth += word.width() + fSpaceAdvance;
        m_vWords.emplace_back(word);
        if (vLineBeginnings.size() == 1 && word.height() > nFirstLineMaxHeight)
            nFirstLineMaxHeight = word.height();
        ++nIndex;
        nCharCount += word.word().size();
    }
    adjustPosX();
    vLineBeginnings.back().second = fPosX;
    vLineBeginnings.emplace_back(std::make_pair(m_vWords.size(), -1.0f));

    const auto nNumLines = vLineBeginnings.size() - 1;
    
    // If no height constraint is set, use fPosY == 0.0f, since then the widget
    // automatically adjusts its size to the contents and then of course the alignment
    // is top.
    const float fRawHeight = nNumLines * fontInfo.lineHeight * fPixelHeight;
    auto pHeightConstraint = getSizeConstraint(OrthogonalAxis::VERTICAL);
    if (pHeightConstraint != nullptr) {
        /////////
        // Vertical alignment
        switch (verticalTextAlignment()) {
            case VerticalAlignment::TOP:
                fPosY = -padding().padding(OrthogonalDirection::TOP);
                switch (vertAlignment()) {
                    case VerticalAlignment::TOP:
                        break;
                    case VerticalAlignment::MIDDLE:
                        fPosY += (height() / 2.0f);
                        break;
                    case VerticalAlignment::BOTTOM:
                        fPosY += height();
                        break;
                    default:
                        break;
                }
                break;
            case VerticalAlignment::MIDDLE:
                fPosY = (fRawHeight / 2.0f);
                switch (vertAlignment()) {
                    case VerticalAlignment::TOP:
                        fPosY += -(height() / 2.0f);
                        break;
                    case VerticalAlignment::MIDDLE:
                        break;
                    case VerticalAlignment::BOTTOM:
                        fPosY += (height() / 2.0f) + padding().padding(OrthogonalDirection::BOTTOM);
                        break;
                    default:
                        break;
                }
                break;
            case VerticalAlignment::BOTTOM:
                fPosY = fRawHeight + padding().padding(OrthogonalDirection::BOTTOM);
                switch (vertAlignment()) {
                    case VerticalAlignment::TOP:
                        fPosY -= height();
                        break;
                    case VerticalAlignment::MIDDLE:
                        fPosY -= (height() / 2.0f);
                        break;
                    case VerticalAlignment::BOTTOM:
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
        //////////
    }
    // EXPERIMENTAL: fontInfo.baselineHeight - fontInfo.size, is this correct?
    // We want to get rid of the margin at the top of the first line
    const float fFirstLineOffset = systemServices.getGraphicsSystem()->getNormalizedPixelsVertical(fontInfo.baselineHeight - fontInfo.size);
    fPosY += fFirstLineOffset;
    m_vVertexBuffer.reserve(24 * nCharCount);
    for (size_t i = 0; i < vLineBeginnings.size() - 1; ++i) {
        fPosX = vLineBeginnings.at(i).second;
        // Iterate over words belonging to this line
        for (size_t nIndex = vLineBeginnings[i].first; nIndex < vLineBeginnings[i+1].first; ++nIndex) {
            auto& word = m_vWords.at(nIndex);
            word.setPosX(fPosX);
            word.setPosY(fPosY);
            m_vWords.at(nIndex).generateVertices();
            m_vVertexBuffer.insert(m_vVertexBuffer.end(), word.vertexBuffer().begin(), word.vertexBuffer().end());
            
            fPosX += word.width() + fSpaceAdvance;
        }
        fPosY -= fontInfo.lineHeight * fPixelHeight;
    }

    m_fHeight = (fontInfo.lineHeight * nNumLines) * fPixelHeight + padding().padding(OrthogonalDirection::TOP) + padding().padding(OrthogonalDirection::BOTTOM) - fFirstLineOffset;
}

void TextLabel::render(IWidgetRenderer *pRenderer) {
    if (graphicsDirty()) {
        generate();
    }
    pRenderer->renderWidget(this);
    setGraphicsDirty(false);
}

void TextLabel::onAddedToParent() {
    setColor(glm::vec4{ 0.0f, 0.0f, 0.0f, 0.0f }); // transparent background
    setConstraintsDirty(false,false);
}

}

}
