//
//  ScrollWidget.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 21.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GUI/ScrollWidget.h>
#include <GUI/Button.h>
#include <GUI/Constraints/AlignmentConstraint.h>
#include <GUI/Constraints/DirectSizeConstraint.h>
#include <GUI/Constraints/TwoPointSizeConstraint.h>
#include <GUI/Constraints/MarginConstraint.h>
#include <System/SystemServices.h>
#include <GUI/WidgetManager.h>
#include <System/Pixels.h>
#include <System/Percent.h>
#include <GUI/StackPanel.h>
#include <GUI/Constraints/DynamicConstraint.h>
#include <GUI/ImageView.h>
#include <iostream>
#include <Interfaces/IWidgetRenderer.h>

#include <SDL2/SDL.h>

namespace Ice {

namespace GUI {

ScrollWidget::ScrollWidget() {
    setColor(glm::vec4{ 0.0f, 0.0f, 0.0f, 0.0f });
    setPadding(0.0f, 0.0f, 0.0f, 0.0f);
    setIsTabStoppable(true);
    setIsConstructing(true);
    m_controlColor = systemServices.getWidgetManager().theme().color() + glm::vec4{ .2f, .2f, .2f, .0f };
    
    createScrollContainer(OrthogonalAxis::VERTICAL);
    createScrollContainer(OrthogonalAxis::HORIZONTAL);

    createButtons(OrthogonalAxis::VERTICAL);
    createButtons(OrthogonalAxis::HORIZONTAL);

    createScrollBarButton(OrthogonalAxis::VERTICAL);
    createScrollBarButton(OrthogonalAxis::HORIZONTAL);
    
    setOnWindowResizedFunc([](Widget* w, int, int) {
        // Scroll back to the original position in case the current scroll positions are
        // invalid after the resize
        ScrollWidget* pScroll = static_cast<ScrollWidget*>(w);
        pScroll->scroll(OrthogonalAxis::VERTICAL, -pScroll->scrollPosition(OrthogonalAxis::VERTICAL));
        pScroll->scroll(OrthogonalAxis::HORIZONTAL, -pScroll->scrollPosition(OrthogonalAxis::HORIZONTAL));
    });

    setIsConstructing(false);
    
    m_bInitialized = true;
}

void ScrollWidget::createScrollContainer(OrthogonalAxis axis) {
    auto& wm = systemServices.getWidgetManager();
    auto pContainer = wm.createWidget<Widget, true>(this);

    switch (axis) {
    case OrthogonalAxis::VERTICAL:
        m_pScrollBarContainerVertical = pContainer;
        break;
    case OrthogonalAxis::HORIZONTAL:
        m_pScrollBarContainerHorizontal = pContainer;
        break;
    }

    pContainer->setColor(m_controlColor);
    pContainer->setPadding(0_px, 0_px, 0_px, 0_px);

    switch (axis) {
    case OrthogonalAxis::VERTICAL:
    {
        pContainer->setPointConstraint(HorizontalAlignment::RIGHT, std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::RIGHT, false));
        pContainer->setPointConstraint(VerticalAlignment::TOP, std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::TOP, false));
        pContainer->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DirectSizeConstraint<Pixels>>(0_px, false));
        pContainer->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<DynamicSizeConstraint>([](Widget* w) {
            const float fHeight = systemServices.getGraphicsSystem()->getNormalizedPixelsVertical(ScrollWidget::BUTTON_SIZE);
            return w->height() - fHeight;
        }));
    }
        break;
    case OrthogonalAxis::HORIZONTAL:
    {
        pContainer->setPointConstraint(HorizontalAlignment::LEFT, std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::LEFT, false));
        pContainer->setPointConstraint(VerticalAlignment::BOTTOM, std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::BOTTOM, false));
        pContainer->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<DirectSizeConstraint<Pixels>>(0_px, false));
        pContainer->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DynamicSizeConstraint>([](Widget* w) {
            const float fWidth = systemServices.getGraphicsSystem()->getNormalizedPixelsHorizontal(ScrollWidget::BUTTON_SIZE);
            return w->width() - fWidth;
        }));
    }
        break;
    }

}

void ScrollWidget::initContentWidget() {
    m_pContent->setPointConstraint(HorizontalAlignment::LEFT, std::make_unique<DynamicPointConstraint>([](Widget* w) {
        auto pScrollWidget = static_cast<ScrollWidget*>(w);
        return pScrollWidget->left(true) - pScrollWidget->scrollPosition(OrthogonalAxis::HORIZONTAL);
      }));
    m_pContent->setPointConstraint(VerticalAlignment::TOP, std::make_unique<DynamicPointConstraint>([](Widget* w) {
        auto pScrollWidget = static_cast<ScrollWidget*>(w);
        return pScrollWidget->top(true) + pScrollWidget->scrollPosition(OrthogonalAxis::VERTICAL);
    }));
    m_pContent->setClipFrameFunc([](const Widget* w) {
        auto pScrollWidget = static_cast<ScrollWidget*>(w->parent());
        const float l = pScrollWidget->left();
        const float b = pScrollWidget->scrollBarContainer(OrthogonalAxis::HORIZONTAL)->top();
        const float r = pScrollWidget->scrollBarContainer(OrthogonalAxis::VERTICAL)->left();
        const float t = pScrollWidget->top();
        return std::make_tuple(l, b, r, t);
    });
}

/*
 * \name scrollBarHeightFactor
 *
 * \param bRefresh true -> value is recalculated
 *
 * \description Determines the ratio between the displayable content size and the
 *              actual size of the content. This will be used to scale the scroll bar.
 *              The scroll bar size represents the amount of content that can be displayed
 *              in relation to the size of the content.
 *
 * \date 23.12.2020
 */
float ScrollWidget::scrollBarHeightFactor(OrthogonalAxis axis, bool bRefresh) const noexcept {
    switch (axis) {
    case OrthogonalAxis::VERTICAL:
        if (m_fScrollBarHeightFactorVertical < 0.0f || bRefresh) {
            m_fScrollBarHeightFactorVertical = contentHeight() / content()->height();
            m_fScrollBarHeightFactorVertical = std::min(m_fScrollBarHeightFactorVertical, 1.0f);
        }
        return m_fScrollBarHeightFactorVertical;
    case OrthogonalAxis::HORIZONTAL:
        if (m_fScrollBarHeightFactorHorizontal < 0.0f || bRefresh) {
            m_fScrollBarHeightFactorHorizontal = contentWidth() / content()->width();
            m_fScrollBarHeightFactorHorizontal = std::min(m_fScrollBarHeightFactorHorizontal, 1.0f);
        }
        return m_fScrollBarHeightFactorHorizontal;
    }
    return -1.0f;
}

/*
 * \name scrollContainerToContentFactor
 *
 * \param bRefresh true -> value is recalculated
 *
 * \description Determines the ratio between the size of the content and the size of the
 *              scroll container (space between the 2 scroll buttons). The size of the scroll
 *              container represents the size of the contents. This ratio is used to convert between
 *              an offset in scroll container space to screen (NDC) space (which is used for all the widgets)
 *
 * \date 23.12.2020
 */
float ScrollWidget::scrollContainerToContentFactor(OrthogonalAxis axis, bool bRefresh) const noexcept {
    const auto fSizeBetweenButtons = sizeScrollBarContainerBetweenButtons(axis);
    switch (axis) {
    case OrthogonalAxis::VERTICAL:
        if (m_fScrollBarToContentFactorVertical < 0.0f || bRefresh) {
            m_fScrollBarToContentFactorVertical = fSizeBetweenButtons / content()->height();
        }
        return m_fScrollBarToContentFactorVertical;
    case OrthogonalAxis::HORIZONTAL:
        if (m_fScrollBarToContentFactorHorizontal < 0.0f || bRefresh) {
            m_fScrollBarToContentFactorHorizontal = fSizeBetweenButtons / content()->width();
        }
        return m_fScrollBarToContentFactorHorizontal;
    }
    return -1.0f;
}


void ScrollWidget::createButtons(OrthogonalAxis axis) noexcept {
    auto pContainer = scrollBarContainer(axis);

    auto& wm = systemServices.getWidgetManager();
    if (axis == OrthogonalAxis::VERTICAL) {
        m_pTopButton = static_cast<Button*>(wm.createWidget<Button, true>(pContainer));
        m_pTopButton->setPadding(0.0f, 0.0f, 0.0f, 0.0f);
        m_pTopButton->setColor(m_controlColor);

        m_pTopButton->setPointConstraint(HorizontalAlignment::RIGHT, std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::RIGHT));
        m_pTopButton->setPointConstraint(VerticalAlignment::TOP, std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::TOP));
        m_pTopButton->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));
        m_pTopButton->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<DirectSizeConstraint<Pixels>>(25_px));

        std::string strImagePath = systemServices.getConfig()->prependAssetPath("GUI/ScrollButtonUp.png");
//        std::string strImagePath{ Config::BasePath };
//        strImagePath.append("/Assets/GUI/ScrollButtonUp.png");

        auto pImage = static_cast<ImageView*>(wm.createWidget<ImageView, true>(topButton(axis), strImagePath));
        pImage->setContentMode(ImageView::ContentMode::FILL);
        pImage->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<TwoPointSizeConstraint>(pImage->getPointConstraint(OrthogonalAxis::HORIZONTAL), std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::RIGHT)));
        pImage->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<TwoPointSizeConstraint>(pImage->getPointConstraint(OrthogonalAxis::VERTICAL), std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::BOTTOM)));
        pImage->setThemable(true);

        m_pBottomButton = static_cast<Button*>(wm.createWidget<Button, true>(pContainer));
        m_pBottomButton->setPadding(0.0f, 0.0f, 0.0f, 0.0f);

        m_pBottomButton->setColor(glm::vec4{ .5f, .5f, .5f, .95f });

        m_pBottomButton->setPointConstraint(HorizontalAlignment::RIGHT, std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::RIGHT));
        m_pBottomButton->setPointConstraint(VerticalAlignment::BOTTOM, std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::BOTTOM));
        m_pBottomButton->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));
        m_pBottomButton->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<DirectSizeConstraint<Pixels>>(25_px));
        m_pBottomButton->setColor(m_controlColor);

//        strImagePath = Config::BasePath;
//        strImagePath.append("/Assets/GUI/ScrollButtonDown.png");
        strImagePath = systemServices.getConfig()->prependAssetPath("GUI/ScrollButtonDown.png");

        pImage = static_cast<ImageView*>(wm.createWidget<ImageView, true>(bottomButton(axis), strImagePath));
        pImage->setContentMode(ImageView::ContentMode::FILL);
        pImage->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<TwoPointSizeConstraint>(pImage->getPointConstraint(OrthogonalAxis::HORIZONTAL), std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::RIGHT)));
        pImage->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<TwoPointSizeConstraint>(pImage->getPointConstraint(OrthogonalAxis::VERTICAL), std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::BOTTOM)));
        pImage->setThemable(true);
    }
    else if (axis == OrthogonalAxis::HORIZONTAL) {
        m_pLeftButton = static_cast<Button*>(wm.createWidget<Button, true>(pContainer));
        m_pLeftButton->setPadding(0.0f, 0.0f, 0.0f, 0.0f);
        m_pLeftButton->setColor(glm::vec4{ .5f, .5f, .5f, .95f });

        m_pLeftButton->setPointConstraint(HorizontalAlignment::LEFT, std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::LEFT));
        m_pLeftButton->setPointConstraint(VerticalAlignment::BOTTOM, std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::BOTTOM));
        m_pLeftButton->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));
        m_pLeftButton->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DirectSizeConstraint<Pixels>>(25_px));
        m_pLeftButton->setColor(m_controlColor);

//        std::string strImagePath{ Config::BasePath };
//        strImagePath.append("/Assets/GUI/ScrollButtonLeft.png");
        std::string strImagePath = systemServices.getConfig()->prependAssetPath("GUI/ScrollButtonLeft.png");

        auto pImage = static_cast<ImageView*>(wm.createWidget<ImageView, true>(topButton(axis), strImagePath));
        pImage->setContentMode(ImageView::ContentMode::FILL);
        pImage->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<TwoPointSizeConstraint>(pImage->getPointConstraint(OrthogonalAxis::HORIZONTAL), std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::RIGHT)));
        pImage->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<TwoPointSizeConstraint>(pImage->getPointConstraint(OrthogonalAxis::VERTICAL), std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::BOTTOM)));
        pImage->setThemable(true);

        m_pRightButton = static_cast<Button*>(wm.createWidget<Button, true>(pContainer));
        m_pRightButton->setPadding(0.0f, 0.0f, 0.0f, 0.0f);

        m_pRightButton->setColor(glm::vec4{ .5f, .5f, .5f, .95f });

        m_pRightButton->setPointConstraint(HorizontalAlignment::RIGHT, std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::RIGHT));
        m_pRightButton->setPointConstraint(VerticalAlignment::BOTTOM, std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::BOTTOM));
        m_pRightButton->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));
        m_pRightButton->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DirectSizeConstraint<Pixels>>(25_px));
        m_pRightButton->setColor(m_controlColor);

//        strImagePath = Config::BasePath;
//        strImagePath.append("/Assets/GUI/ScrollButtonRight.png");
        strImagePath = systemServices.getConfig()->prependAssetPath("GUI/ScrollButtonRight.png");

        pImage = static_cast<ImageView*>(wm.createWidget<ImageView>(bottomButton(axis), strImagePath));
        pImage->setContentMode(ImageView::ContentMode::FILL);
        pImage->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<TwoPointSizeConstraint>(pImage->getPointConstraint(OrthogonalAxis::HORIZONTAL), std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::RIGHT)));
        pImage->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<TwoPointSizeConstraint>(pImage->getPointConstraint(OrthogonalAxis::VERTICAL), std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::BOTTOM)));
        pImage->setThemable(true);
    }
    topButton(axis)->setOnMouseButtonFunc([axis](Widget* w, MouseButton mb, bool bPressed, bool& bHandled) {
        ScrollWidget* pScrollWidget = static_cast<ScrollWidget*>(w->parent()->parent());
        pScrollWidget->scroll(axis, -pScrollWidget->content()->height() * .1f);
        });
    bottomButton(axis)->setOnMouseButtonFunc([axis](Widget* w, MouseButton mb, bool bPressed, bool& bHandled) {
        ScrollWidget* pScrollWidget = static_cast<ScrollWidget*>(w->parent()->parent());
        pScrollWidget->scroll(axis, pScrollWidget->content()->height() * .1f);
        });
}

void ScrollWidget::createScrollBarButton(OrthogonalAxis axis) {
    auto pContainer = scrollBarContainer(axis);

    auto& wm = systemServices.getWidgetManager();
    if (axis == OrthogonalAxis::VERTICAL) {
        m_pScrollBarButtonVertical = static_cast<Button*>(wm.createWidget<Button, true>(pContainer));
        m_pScrollBarButtonVertical->setPointConstraint(HorizontalAlignment::RIGHT, std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::RIGHT));
        m_pScrollBarButtonVertical->setPointConstraint(VerticalAlignment::TOP, std::make_unique<MarginConstraint<NormalizedPixels>>(OrthogonalDirection::BOTTOM, 0.0_npx, false, topButton(axis)));
        m_pScrollBarButtonVertical->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));
        m_pScrollBarButtonVertical->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<DynamicSizeConstraint>([axis](Widget* w) {
            auto pScrollWidget = static_cast<ScrollWidget*>(w->parent());
            float fRet = pScrollWidget->scrollBarHeightFactor(axis) * (pScrollWidget->topButton(axis)->bottom(false) - pScrollWidget->bottomButton(axis)->top(false));
            return fRet;
            }));
        m_pScrollBarButtonVertical->setColor(systemServices.getWidgetManager().theme().foregroundColor());
        scrollBarButton(OrthogonalAxis::VERTICAL)->setOnMouseOverFunc([](Widget* inst, float x, float y, float offsetX, float offsetY, std::uint8_t buttonMask, bool& bHandled) {
            if (buttonMask & (std::uint8_t)MouseButton::LEFT) {
                Button* pBarButton = static_cast<Button*>(inst);
                ScrollWidget* pScrollWidget = static_cast<ScrollWidget*>(pBarButton->parent()->parent());
                pScrollWidget->scroll(OrthogonalAxis::VERTICAL, offsetY);
            }
            });
    }
    else if (axis == OrthogonalAxis::HORIZONTAL) {
        m_pScrollBarButtonHorizontal = static_cast<Button*>(wm.createWidget<Button, true>(pContainer));
        m_pScrollBarButtonHorizontal->setPointConstraint(VerticalAlignment::BOTTOM, std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::BOTTOM));
        m_pScrollBarButtonHorizontal->setPointConstraint(HorizontalAlignment::LEFT, std::make_unique<MarginConstraint<NormalizedPixels>>(OrthogonalDirection::RIGHT, 0.0_npx, false, m_pLeftButton));
        m_pScrollBarButtonHorizontal->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));
        m_pScrollBarButtonHorizontal->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DynamicSizeConstraint>([axis](Widget* w) {
            auto pScrollWidget = static_cast<ScrollWidget*>(w->parent());
            float fRet = pScrollWidget->scrollBarHeightFactor(axis) * abs(pScrollWidget->topButton(axis)->right(false) - pScrollWidget->bottomButton(axis)->left(false));
            return fRet;
            }));
        m_pScrollBarButtonHorizontal->setColor(systemServices.getWidgetManager().theme().foregroundColor());
        scrollBarButton(OrthogonalAxis::HORIZONTAL)->setOnMouseOverFunc([](Widget* inst, float x, float y, float offsetX, float offsetY, std::uint8_t buttonMask, bool& bHandled) {
            if (buttonMask & (std::uint8_t)MouseButton::LEFT) {
                Button* pBarButton = static_cast<Button*>(inst);
                ScrollWidget* pScrollWidget = static_cast<ScrollWidget*>(pBarButton->parent()->parent());
                pScrollWidget->scroll(OrthogonalAxis::HORIZONTAL, offsetX);
            }
            });
    }
}

void ScrollWidget::scroll(OrthogonalAxis axis, float offset) {
    const float fSizeBetweenButtons = sizeScrollBarContainerBetweenButtons(axis);
    
    auto pConstraint = static_cast<MarginConstraint<NormalizedPixels>*>(scrollBarButton(axis)->getPointConstraint(axis));
    float fNewValueScrollBarSpace = (float)pConstraint->margin() - offset;
    
    // We're using a margin constraint in relation to the bottom of the top button (or right of the left button).
    // Zero means the scroll bar is at the top. Scrolling down means the scroll bar is moved
    // down, therefore the min value must be negative.
    float fLimitValueScrollBarSpace = fSizeBetweenButtons - (fSizeBetweenButtons * scrollBarHeightFactor(axis));
    fLimitValueScrollBarSpace *= -1.0f;

    fNewValueScrollBarSpace = std::min(0.0f, fNewValueScrollBarSpace);
    fNewValueScrollBarSpace = std::max(fLimitValueScrollBarSpace, fNewValueScrollBarSpace);

    pConstraint->setMargin(NormalizedPixels{ fNewValueScrollBarSpace });
    
    const float fNewScrollPosition = -fNewValueScrollBarSpace * (1.0f / scrollContainerToContentFactor(axis));
    // Update scroll position (negative, because in order to move the scroll bar down, we need a negative value above,
    // so here we negate it to get a positive scroll position)
    switch (axis) {
    case OrthogonalAxis::VERTICAL:
        m_fScrollPositionVertical = fNewScrollPosition;
        break;
    case OrthogonalAxis::HORIZONTAL:
        m_fScrollPositionHorizontal = fNewScrollPosition;
        break;
    }

    scrollBarButton(axis)->setConstraintsDirty(false, true);   // next time, fetch new height info
    //content()->refreshConstraints(); // scrolled up, also fetch new info
    content()->setConstraintsDirty(false, true);
    //for (const auto& info : content()->children())
    //    info.child()->refreshConstraints();
}
        
float ScrollWidget::sizeScrollBarContainerBetweenButtons(OrthogonalAxis axis) const {
    switch (axis) {
    case OrthogonalAxis::VERTICAL:
        return topButton(axis)->bottom(false) - bottomButton(axis)->top(false);
    case OrthogonalAxis::HORIZONTAL:
        return abs(topButton(axis)->right(false) - bottomButton(axis)->left(false));
    }
    return -1.0f;
}

void ScrollWidget::refreshConstraints() {
    Widget::refreshConstraints();
    scrollBarHeightFactor(OrthogonalAxis::VERTICAL, true);
    scrollBarHeightFactor(OrthogonalAxis::HORIZONTAL, true);
    scrollContainerToContentFactor(OrthogonalAxis::VERTICAL, true);
    scrollContainerToContentFactor(OrthogonalAxis::HORIZONTAL, true);
    adjustScrollers();
}

/*
 * \name ScrollWidget::addChild
 *
 * \description After the object has been initialized, redirect added children to the
 *              vertical stack panel
 *
 * \date 26.12.2020
 */
void ScrollWidget::addChild(Widget* w, size_t nDrawOrder) {
    if (m_pContent != nullptr)
        throw std::runtime_error("ScrollWidget can only have one child element!");
    Widget::addChild(w, nDrawOrder);
    if (m_bInitialized) {
        
        w->setIsTabStoppable(false);
        //content()->addChild(w, nDrawOrder);
        m_pContent = w;

        //adjustScrollers();
        initContentWidget();
    }
}

void ScrollWidget::adjustScrollers() {
    auto verticalPixels = 0_px;
    auto horizontalPixels = 0_px;
    if (content()->height() > contentHeight()) {
        verticalPixels = ScrollWidget::BUTTON_SIZE;
    }
    auto pConst = static_cast<DirectSizeConstraint<Pixels>*>(scrollBarContainer(OrthogonalAxis::VERTICAL)->getSizeConstraint(OrthogonalAxis::HORIZONTAL));
    pConst->setSize(verticalPixels);
    //scrollBarContainer(OrthogonalAxis::VERTICAL)->refreshConstraints();
    // Propagate new size of the container to all the children
    scrollBarContainer(OrthogonalAxis::VERTICAL)->setConstraintsDirty(false, true);

    if (content()->width() > contentWidth()) {
        horizontalPixels = ScrollWidget::BUTTON_SIZE;
    }
    pConst = static_cast<DirectSizeConstraint<Pixels>*>(scrollBarContainer(OrthogonalAxis::HORIZONTAL)->getSizeConstraint(OrthogonalAxis::VERTICAL));
    pConst->setSize(horizontalPixels);
    //scrollBarContainer(OrthogonalAxis::HORIZONTAL)->setConstraintsDirty(false, true);
    // Propagate new size of the container to all the children
    scrollBarContainer(OrthogonalAxis::HORIZONTAL)->setConstraintsDirty(false, true);
}

void ScrollWidget::render(IWidgetRenderer* pRenderer) {
    //adjustScrollers();
    pRenderer->renderWidget(this);
    setGraphicsDirty(false);
}

}

}
