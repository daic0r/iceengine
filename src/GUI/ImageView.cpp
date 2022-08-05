//
//  ImageView.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 23.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GUI/ImageView.h>
#include <Interfaces/IWidgetRenderer.h>
#include <Interfaces/IGraphicsSystem.h>
#include <System/SystemServices.h>
#include <System/BaseTexture.h>
#include <GUI/Constraints/DirectSizeConstraint.h>
#include <GUI/WidgetManager.h>

namespace Ice {

namespace GUI {

ImageView::ImageView(const std::shared_ptr<Image>& pImg, bool bAddSizeConstraints)
    : m_pImage{ pImg }
{
    _init(bAddSizeConstraints);
}

ImageView::ImageView(const std::string& strFile, bool bAddSizeConstraints) {
    m_pImage = std::make_shared<Image>(strFile);
    _init(bAddSizeConstraints);
}

bool ImageView::themable() const noexcept {
    return foregroundColor().a > 0.0f; 
}

void ImageView::setThemable(bool b) noexcept {
    if (b)
        setForegroundColor(systemServices.getWidgetManager().theme().foregroundColor()); // <-- theme color
    else
        setForegroundColor(glm::vec4{ -1.0f, -1.0f, -1.0f, -1.0f });
}

void ImageView::_init(bool bAddSizeConstraints) {
    setPadding(0.0f, 0.0f, 0.0f, 0.0f);
    setColor(glm::vec4{ 0.0f, 0.0f, 0.0f, 0.0f });
    setForegroundColor(glm::vec4{ -1.0f, -1.0f, -1.0f, -1.0f });
    if (bAddSizeConstraints) {
        // Automatically set size constraints
        // Can be overriden by user later
        setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DirectSizeConstraint<Pixels>>(Pixels(image()->width())));
        setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<DirectSizeConstraint<Pixels>>(Pixels(image()->height())));
    }
}

void ImageView::render(IWidgetRenderer* pRenderer) {
    pRenderer->renderWidget(this);
    setGraphicsDirty(false);
}

std::pair<glm::vec2, glm::vec2> ImageView::texCoord() const noexcept {
    glm::vec2 minPoint, maxPoint;

    const float fTexWidth = systemServices.getGraphicsSystem()->getNormalizedPixelsHorizontal(image()->data()->width());
    const float fTexHeight = systemServices.getGraphicsSystem()->getNormalizedPixelsVertical(image()->data()->height());
    const auto fWidthRatio = contentWidth() / fTexWidth;
    const auto fHeightRatio = contentHeight() / fTexHeight;

    auto getScaledTexCoords = [fTexWidth, fTexHeight, this](float fFactor, glm::vec2& minPoint, glm::vec2& maxPoint) {
        const float x = (1.0f - fTexWidth * fFactor / contentWidth()) / 2.0f;
        const float y = (1.0f - fTexHeight * fFactor / contentHeight()) / 2.0f;
        minPoint = glm::vec2{ -x, -y };
        maxPoint = glm::vec2{ 1.0f + x, 1.0f + y };
    };
    
    switch (contentMode()) {
        case ContentMode::FILL:
            minPoint = glm::vec2{ 0.0f, 0.0f };
            maxPoint = glm::vec2{ 1.0f, 1.0f };
            break;
        case ContentMode::ORIGINAL:
            // Calculate tex coordinates so that the image is centered
            getScaledTexCoords(1.0f, minPoint, maxPoint);
            break;
        case ContentMode::SCALE_TO_FIT:
        {
            const float fInvGoverningRatio = std::min(fWidthRatio, fHeightRatio);
            getScaledTexCoords(fInvGoverningRatio, minPoint, maxPoint);
        }
            break;
        case ContentMode::FIT_HORIZONTALLY:
        {
            const float fInvGoverningRatio = fWidthRatio;
            getScaledTexCoords(fInvGoverningRatio, minPoint, maxPoint);
        }
            break;
        case ContentMode::FIT_VERTICALLY:
        {
            const float fInvGoverningRatio = fHeightRatio;
            getScaledTexCoords(fInvGoverningRatio, minPoint, maxPoint);
        }
            break;
    }
    if (minPoint.x > maxPoint.x)
        std::swap(minPoint.x, maxPoint.x);
    if (minPoint.y > maxPoint.y)
        std::swap(maxPoint.y, maxPoint.y);
    return std::make_pair(minPoint, maxPoint);
}

}

}
