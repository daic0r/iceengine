//
//  WidgetView.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 30.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GUI/WidgetView.h>
#include <Interfaces/IWidgetRenderer.h>
#include <iostream>

namespace Ice {

namespace GUI {

void WidgetView::swapConstraints() noexcept {
    std::swap(m_pX, widget()->m_pX);
    std::swap(m_pY, widget()->m_pY);
    std::swap(m_pWidth, widget()->m_pWidth);
    std::swap(m_pHeight, widget()->m_pHeight);
    std::swap(m_paddingFunc, widget()->m_paddingFunc);
    std::swap(m_clipFrameFunc, widget()->m_clipFrameFunc);
    std::swap(m_fXCached, widget()->m_fXCached);
    std::swap(m_fYCached, widget()->m_fYCached);
    std::swap(m_fWidthCached, widget()->m_fWidthCached);
    std::swap(m_fHeightCached, widget()->m_fHeightCached);
    std::swap(m_pParent, widget()->m_pParent);
    std::swap(m_color, widget()->m_color);
}

void WidgetView::refreshConstraints(Widget* w) {
    w->refreshConstraints();
    for (const auto& info : w->children()) {
        refreshConstraints(info.pChild);
    }
}

void WidgetView::render(IWidgetRenderer *pRenderer) {
    if (widget() == nullptr) {
        pRenderer->renderWidget(this);
        return;
    }

    swapConstraints();
    refreshConstraints(m_pWidget);
    m_pWidget->render(pRenderer);
    swapConstraints();
    refreshConstraints(m_pWidget);
    setGraphicsDirty(false);
}

}

}
