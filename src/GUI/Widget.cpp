//
//  Widget.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 13.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GUI/Widget.h>
#include <iostream>
#include <Interfaces/IWidgetRenderer.h>
#include <Interfaces/IGraphicsSystem.h>
#include <System/SystemServices.h>
#include <GUI/Constraints/AlignmentConstraint.h>
#include <GUI/Constraints/DirectSizeConstraint.h>
#include <System/Percent.h>
#include <algorithm>
#include <GUI/WidgetManager.h>
#include <System/ThreadPool.h>

namespace Ice {

namespace GUI {

Widget::Widget()
    : m_handle{ nNextWidgetHandle++ } {
    
    m_paddingFunc = [](const Widget* inst) {
        static constexpr auto p = 5_px;
        
        // Do not make this dependent on the width or height, this will lead to problems
        // with circular dependencies!
        const float fLeft = systemServices.getGraphicsSystem()->getNormalizedPixelsHorizontal(p);
        const float fTop = systemServices.getGraphicsSystem()->getNormalizedPixelsVertical(p);
        return Padding{ fLeft, fTop, fLeft, fTop };
    };
        
//    m_clipFrameFunc = [](const Widget* inst) {
//        return inst->parent() != nullptr ? inst->parent()->clipFrame() : inst->extents();
//    };
}

/*
 * \name addChild
 *
 * \param WidgetHandle The handle of the widget
 * \param size_t The draw order (smaller is drawn first)
 *
 * \description Adds the widget with handle h to the list of children and then sorts
 *              the resulting vector by draw order
 *
 * \date 14.12.2020
 */
void Widget::addChild(Widget* pChild, size_t nDrawOrder) {
    if (std::find_if(m_vChildren.cbegin(), m_vChildren.cend(), [pChild](const sChildInfo& info) { return info.pChild == pChild; }) != m_vChildren.cend())
        throw std::runtime_error("Widget is already a child of this widget");
    
    // Safety net: if pChild was initially created without a parent but is now added
    // to this widget, we must remove it from the list of root level widgets.
    // This will not be the case if addChild is invoked from createWidget(), of course.
    //////////////////////////////////////////////////////////////////////////
    if (pChild->parent() == nullptr)
        systemServices.getWidgetManager().removeRootLevelWidget(pChild);
    //////////////////////////////////////////////////////////////////////////
    pChild->setParent(this);
    if (pChild->getPointConstraint(OrthogonalAxis::HORIZONTAL) == nullptr) {
        pChild->setPointConstraint(HorizontalAlignment::LEFT, std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::LEFT));
    }
    if (pChild->getPointConstraint(OrthogonalAxis::VERTICAL) == nullptr) {
        pChild->setPointConstraint(VerticalAlignment::TOP, std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::TOP));
    }
    m_vChildren.emplace_back(sChildInfo{ pChild, nDrawOrder });
    std::sort(m_vChildren.begin(), m_vChildren.end(), [](const sChildInfo& a, const sChildInfo& b) { return a.nDrawOrder < b.nDrawOrder; });
}

void Widget::removeChild(Widget* pWidget, bool bMakeRootLevelWidget) {
    m_vChildren.erase(std::remove_if(m_vChildren.begin(), m_vChildren.end(), [pWidget](const auto& info) { return info.pChild == pWidget; }), m_vChildren.end());
    // Handle constraints
    pWidget->setParent(nullptr);
    auto pX = pWidget->getPointConstraint(OrthogonalAxis::HORIZONTAL);
    pX->setParent(nullptr);
    auto pY = pWidget->getPointConstraint(OrthogonalAxis::VERTICAL);
    pY->setParent(nullptr);
    auto pWidth = pWidget->getSizeConstraint(OrthogonalAxis::HORIZONTAL);
    pWidth->setParent(nullptr);
    auto pHeight = pWidget->getSizeConstraint(OrthogonalAxis::VERTICAL);
    pHeight->setParent(nullptr);
    //////////////////////////////////////////////
    if (bMakeRootLevelWidget) {
        systemServices.getWidgetManager().m_vRootLevelWidgets.push_back(pWidget);
    }
}

void Widget::setPointConstraint(HorizontalAlignment a, std::unique_ptr<PointConstraint> pCnst) {
    if (a == HorizontalAlignment::NONE)
        throw std::runtime_error("Alignment for point constraint must not be NONE");
    
    if (!pCnst) {
        m_pX.second.reset();
        return;
    }
    
//    if (pCnst->direction() != OrthogonalDirection::LEFT && pCnst->direction() != OrthogonalDirection::RIGHT)
//        throw std::runtime_error("Horizontal PointConstraint orientation must either be LEFT or RIGHT");
    
    if (pCnst->parent() == nullptr)
        pCnst->setParent(parent() != nullptr ? parent() : nullptr);
    m_pX = std::make_pair(a, std::move(pCnst));
    setConstraintsDirty(true, false);
}

void Widget::setPointConstraint(VerticalAlignment a, std::unique_ptr<PointConstraint> pCnst) {
    if (a == VerticalAlignment::NONE)
        throw std::runtime_error("Alignment for point constraint must not be NONE");
    
    if (!pCnst) {
        m_pY.second.reset();
        return;
    }
    
//    if (pCnst->direction() != OrthogonalDirection::TOP && pCnst->direction() != OrthogonalDirection::BOTTOM)
//        throw std::runtime_error("Vertical PointConstraint orientation must either be TOP or BOTTOM");
    
    if (pCnst->parent() == nullptr)
        pCnst->setParent(parent() != nullptr ? parent() : nullptr);
    m_pY = std::make_pair(a, std::move(pCnst));
    setConstraintsDirty(true, false);
}

void Widget::setSizeConstraint(OrthogonalAxis axis, std::unique_ptr<SizeConstraint> pCnst) {
    if (axis == OrthogonalAxis::NONE)
        throw std::runtime_error("Axis must not be NONE");
    
    if (!pCnst) {
        if (axis == OrthogonalAxis::HORIZONTAL)
            m_pWidth.reset();
        else
        if (axis == OrthogonalAxis::VERTICAL)
            m_pHeight.reset();
        return;
    }
    
    if (pCnst->parent() == nullptr)
        pCnst->setParent(parent() != nullptr ? parent() : nullptr);
    pCnst->setAxis(axis);
    switch (axis) {
        case OrthogonalAxis::HORIZONTAL:
            m_pWidth = std::move(pCnst);
            break;
        case OrthogonalAxis::VERTICAL:
            m_pHeight = std::move(pCnst);
            break;
        default:
            break;
    }
    setConstraintsDirty(true, false);
}

float Widget::top(bool bWithPadding) const {
    float fRet{ 0.0f };
    switch (m_pY.first) {
        case VerticalAlignment::TOP:
            fRet = posY();
            break;
        case VerticalAlignment::MIDDLE:
            fRet = posY() + (height() / 2.0f);
            break;
        case VerticalAlignment::BOTTOM:
            fRet = posY() + height();
            break;
        case VerticalAlignment::NONE:
            throw std::runtime_error("Alignment NONE not allowed in this context");
    }
    if (bWithPadding) {
        static const auto pG = systemServices.getGraphicsSystem();
        fRet -= pG->getNormalizedPixelsVertical(padding().padding(OrthogonalDirection::TOP));
    }
    return fRet;
}

float Widget::bottom(bool bWithPadding) const {
    float fRet{ 0.0f };
    switch (m_pY.first) {
        case VerticalAlignment::TOP:
            fRet = posY() - height();
            break;
        case VerticalAlignment::MIDDLE:
            fRet = posY() - (height() / 2.0f);
            break;
        case VerticalAlignment::BOTTOM:
            fRet = posY();
            break;
        case VerticalAlignment::NONE:
            throw std::runtime_error("Alignment NONE not allowed in this context");
    }
    if (bWithPadding) {
        static const auto pG = systemServices.getGraphicsSystem();
        fRet += pG->getNormalizedPixelsVertical(padding().padding(OrthogonalDirection::BOTTOM));
    }
    return fRet;
}

float Widget::left(bool bWithPadding) const {
    float fRet{ 0.0f };
    switch (m_pX.first) {
        case HorizontalAlignment::LEFT:
            fRet = posX();
            break;
        case HorizontalAlignment::CENTER:
            fRet = posX() - (width() / 2.0f);
            break;
        case HorizontalAlignment::RIGHT:
            fRet = posX() - width();
            break;
        case HorizontalAlignment::NONE:
            throw std::runtime_error("Alignment NONE not allowed in this context");
    }
    if (bWithPadding) {
        static const auto pG = systemServices.getGraphicsSystem();
        fRet += pG->getNormalizedPixelsHorizontal(padding().padding(OrthogonalDirection::LEFT));
    }
    return fRet;
}

float Widget::right(bool bWithPadding) const {
    float fRet{ 0.0f };
    switch (m_pX.first) {
        case HorizontalAlignment::LEFT:
            fRet = posX() + width();
            break;
        case HorizontalAlignment::CENTER:
            fRet = posX() + (width() / 2.0f);
            break;
        case HorizontalAlignment::RIGHT:
            fRet = posX();
            break;
        case HorizontalAlignment::NONE:
            throw std::runtime_error("Alignment NONE not allowed in this context");
    }
    if (bWithPadding) {
        static const auto pG = systemServices.getGraphicsSystem();
        fRet -= pG->getNormalizedPixelsHorizontal(padding().padding(OrthogonalDirection::RIGHT));
    }
    return fRet;
}

PointConstraint* Widget::getPointConstraint(OrthogonalAxis a) {
    switch (a) {
        case OrthogonalAxis::HORIZONTAL:
            return m_pX.second.get();
        case OrthogonalAxis::VERTICAL:
            return m_pY.second.get();
        default:
            return nullptr;
    }
}

SizeConstraint* Widget::getSizeConstraint(OrthogonalAxis a) {
    switch (a) {
        case OrthogonalAxis::HORIZONTAL:
            return m_pWidth.get();
        case OrthogonalAxis::VERTICAL:
            return m_pHeight.get();
        default:
            return nullptr;
    }
}

// x, y here are assumed to be in screen NDC [-1;1],[-1;1]
// Conversion to widget-local coords (also [-1;1],[-1;1]) can be done using Widget::globalToLocalCoords()
// Testing if screen NDC coords are inside the widget bounds can be done using Widget::isInsideWidget(float, float)
void Widget::onMouseOver(float x, float y, float offsetX, float offsetY, std::uint8_t buttonMask, bool& bHandled) noexcept {
 //   threadPool.queue([&]() {
        if (m_mouseOverFunc) {
            m_mouseOverFunc(this, x, y, offsetX, offsetY, buttonMask, bHandled);
        }
 //   });
    
    // "Bubble" up to the parents
    if (parent() != nullptr)
        parent()->onMouseOver(x, y, offsetX, offsetY, buttonMask, bHandled);
}

void Widget::onMouseEnter(bool& bHandled) noexcept {
    if (!isMouseOver()) {
        if (m_mouseEnterFunc)
            m_mouseEnterFunc(this, bHandled);
    }

    // "Bubble" up to the parents
    isMouseOver() = true;
    if (parent() != nullptr && !parent()->isMouseOver()) {
        if (!parent()->isMouseOver())
            parent()->onMouseEnter(bHandled);
    }
}

void Widget::onMouseLeave(bool& bHandled) noexcept {
    if (isMouseOver()) {
        if (m_mouseLeaveFunc)
            m_mouseLeaveFunc(this, bHandled);
    }

    isMouseOver() = false;
    // "Tunnel" down to the children
    for (const auto& info : children()) {
        if (info.child()->isMouseOver())
            info.child()->onMouseLeave(bHandled);
    }
}

void Widget::onMouseButton(MouseButton mb, bool bPressed, bool& bHandled) noexcept {
    if (m_mouseButtonFunc)
        m_mouseButtonFunc(this, mb, bPressed, bHandled);

    if (parent() != nullptr)
        parent()->onMouseButton(mb, bPressed, bHandled);
}

void Widget::onFocusReceived() noexcept {
    if (m_focusReceivedFunc)
        m_focusReceivedFunc(this);
    setHasFocus(true);

    if (!isTabStoppable() && parent() != nullptr && !parent()->hasFocus())
        parent()->onFocusReceived();
}

void Widget::onFocusLost() noexcept {
    if (m_focusLostFunc)
        m_focusLostFunc(this);
    setHasFocus(false);

    for (const auto& info : children())
        info.child()->onFocusLost();
}

void Widget::onWindowResized(int w, int h) noexcept {
    refreshConstraints();

    // "Tunnel" down to the children
    for (const auto& childInfo : children()) {
        childInfo.child()->onWindowResized(w, h);
    }

    refreshConstraints();
    
    //threadPool.queue([&]() {
        if (m_windowResizedFunc)
            m_windowResizedFunc(this, w, h);
    //});
}

void Widget::onTextInput(const Widget::sTextInputInfo& ti) noexcept {
    if (m_textInputFunc)
        m_textInputFunc(this, ti);
}

void Widget::render(IWidgetRenderer *pRenderer) {
    pRenderer->renderWidget(this);
    setGraphicsDirty(false);
}

std::tuple<float,float,float,float> Widget::extents(bool bRefresh) const noexcept {
    //if (bRefresh || graphicsDirty() || /*constraintsDirty() ||*/ !m_extentsCached.has_value()) {
        const auto l = left(false);
        const auto b = bottom(false);
        return std::make_tuple(l, b, l+width(), b+height());
    //}
    //return m_extentsCached.value();
}

// x, y here are assumed to be in screen NDC [-1;1],[-1;1]
bool Widget::isInsideWidget(float x, float y) const noexcept {
    const auto [cl, cb, cr, ct] = clipFrame();
    const auto [el, eb, er, et] = extents();
    const auto l = std::max(cl, el);
    const auto b = std::max(cb, eb);
    const auto r = std::min(cr, er);
    const auto t = std::min(ct, et);
    return (x >= l && x <= r && y >= b && y <= t);
}

std::pair<float, float> Widget::globalToLocalCoords(float x, float y) const noexcept {
    const auto w = width();
    const auto h = height();
    x -= left(false) + (w / 2.0f);   // center around 0
    y -= bottom(false) + (h / 2.0f); // center around 0
    x /= w / 2.0f; // rescale from -.5-.5 to -1-1
    y /= h / 2.0f; // rescale from -.5-.5 to -1-1
    // x will now be in the range -1 on the left to 1 on the right
    // y will now be in the range -1 on the bottom and 1 on the right
    return std::make_pair(x, y);
}

/*!
* \brief
* [Widget::rootParent]
*
* \author Matthias.Gruen
* \date 2021/01/05
* [1/5/2021 Matthias.Gruen]
*/
Widget* Widget::rootParent() noexcept
{
    if (parent() == nullptr)
        return this;

    return parent()->rootParent();
}

template<typename Format>
void Widget::setPadding(Format left, Format top, Format right, Format bottom) noexcept {
    m_paddingFunc = [left, top, right, bottom](const Widget*) {
        return Padding{ left, top, right, bottom };
    };
}
template void Widget::setPadding<float>(float,float,float,float);
template void Widget::setPadding<Pixels>(Pixels, Pixels, Pixels, Pixels);


//#TODO: Check if this is really the solution
std::tuple<float, float, float, float> Widget::clipFrame(bool bRefresh) const noexcept {
    //if (bRefresh || graphicsDirty() || /*constraintsDirty() ||*/ !m_clipFrameCached.has_value()) {
        auto pWidget = this;
        while (!pWidget->m_clipFrameFunc) {
            if (pWidget->parent() == nullptr)
                break;
            pWidget = pWidget->parent();
        }
        if (pWidget && pWidget->m_clipFrameFunc)
            return pWidget->m_clipFrameFunc(pWidget);
        //else
        return pWidget->extents();
    //}
    //return m_clipFrameCached.value();
}

bool Widget::visible(bool bRefresh) const noexcept {
    //if (bRefresh || graphicsDirty() || constraintsDirty() || !m_bVisible.has_value()) {
        auto pWidget = this;
        while (pWidget != nullptr && !pWidget->m_bVisible.has_value()) {
            pWidget = pWidget->parent();
        }
        if (pWidget && pWidget->m_bVisible.has_value())
            return pWidget->m_bVisible.value();

        return true;
    //}
    //return m_bVisible.value_or(true);
}

void Widget::refreshConstraints() {
    width(true); height(true); posX(true); posY(true); clipFrame(true); extents(true);// visible(true);
    m_bConstraintsDirty = false;
}

void Widget::clearConstraintsCache() {
    m_fXCached.reset();
    m_fYCached.reset();
    m_fWidthCached.reset();
    m_fHeightCached.reset();
}

void Widget::addAnimation(AnimationTarget target, float fStartValue, float fDuration, bool bInverse) {
    animations().emplace_back([target, fStartValue, fDuration, bInverse](GUI::Widget* w, float fDeltaTime) {
        static float* pTarget{ nullptr };
        static bool bFirstCall = true;
        static float fTargetValue;

        if (bFirstCall) {
            switch (target) {
            case AnimationTarget::X:
                fTargetValue = w->posX();
                pTarget = &(w->m_fXCached.value());
                break;
            case AnimationTarget::Y:
                fTargetValue = w->posY();
                pTarget = &(w->m_fYCached.value());
                break;
            case AnimationTarget::WIDTH:
                fTargetValue = w->width();
                pTarget = &(w->m_fWidthCached.value());
                break;
            case AnimationTarget::HEIGHT:
                fTargetValue = w->height();
                pTarget = &(w->m_fHeightCached.value());
                break;
            }
            for (auto& info : w->children())
                info.child()->setConstraintsDirty(true, true);

        }
        static float fCurrentValue = bInverse ? fTargetValue : fStartValue;
        static const float fDelta = (fTargetValue - fStartValue) / fDuration;
        static const float fSign = ((bInverse ? -1.0f : 1.0f) * (fTargetValue - fStartValue)) >= 0.0f ? 1.0f : -1.0f;

        fCurrentValue += fSign * fDelta * fDeltaTime;
        bool bDone = bInverse ? fCurrentValue < fStartValue : fCurrentValue > fTargetValue;
        fCurrentValue = bInverse ? std::max(fCurrentValue, fStartValue) : std::min(fCurrentValue, fTargetValue);
        *pTarget = fCurrentValue;

        if (bDone) {
            w->setConstraintsDirty(false, true);
        }
        
        bFirstCall = false;

        return bDone;
    });
}

void Widget::_subSetConstraintsDirty(bool b, Widget* w, bool bPropagate) noexcept {
    w->m_bConstraintsDirty = b;
    w->clearConstraintsCache();

    // This will only refresh the childrens' constraints, because by saying that w is
    // not dirty anymore we state that the currently cached value is current and does not
    // require recalculation.
    // The children need to update their constraints with the parent's constraints, however,
    // now, because the parent's constraints have changed before setting dirty to false above,
    // so we do a complete update refresh for the whole hierarchy from w downwards here.
    if (bPropagate) {
        for (const auto& info : w->m_vChildren) {
            //info.child()->refreshConstraints();
            _subSetConstraintsDirty(b, info.pChild, bPropagate);
        }
    }
}

bool Widget::apply(const std::function<bool(Widget*)>& func) {
    if (func(this))
        return true;

    for (const auto& info : children())
        if (info.child()->apply(func))
            return true;

    return false;
}

template<IsMeasurementUnit T, IsMeasurementUnit U>
void Widget::addPresetConstraints(Widget::ConstraintPreset p, Widget* pParent, T fWidth, U fHeight) {
    switch (p) {
        case Widget::ConstraintPreset::FILL_PARENT:
            setPointConstraint(GUI::HorizontalAlignment::LEFT, std::make_unique<GUI::HorizontalAlignmentConstraint>(GUI::HorizontalAlignment::LEFT, true, pParent));
            setPointConstraint(GUI::VerticalAlignment::TOP, std::make_unique<GUI::VerticalAlignmentConstraint>(GUI::VerticalAlignment::TOP, true, pParent));
            if constexpr(std::is_same_v<T, NormalizedPixels>)
                if (fWidth == -1.0_npx)
                    setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<GUI::DirectSizeConstraint<Percent>>(100.0_pct, true, pParent));
            if constexpr(std::is_same_v<U, NormalizedPixels>)
                if (fHeight == -1.0_npx)
                    setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<GUI::DirectSizeConstraint<Percent>>(100.0_pct, true, pParent));
            if (getSizeConstraint(OrthogonalAxis::HORIZONTAL) == nullptr)
                setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<GUI::DirectSizeConstraint<T>>(fWidth, true, pParent));
            if (getSizeConstraint(OrthogonalAxis::VERTICAL) == nullptr)
                setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<GUI::DirectSizeConstraint<U>>(fHeight, true, pParent));
    }
}
template void Widget::addPresetConstraints<Percent, Percent>(Widget::ConstraintPreset p, Widget* pParent, Percent fWidth, Percent fHeight);
template void Widget::addPresetConstraints<Pixels, Percent>(Widget::ConstraintPreset p, Widget* pParent, Pixels fWidth, Percent fHeight);
template void Widget::addPresetConstraints<Percent, Pixels>(Widget::ConstraintPreset p, Widget* pParent, Percent fWidth, Pixels fHeight);
template void Widget::addPresetConstraints<Pixels, Pixels>(Widget::ConstraintPreset p, Widget* pParent, Pixels fWidth, Pixels fHeight);

template void Widget::addPresetConstraints<NormalizedPixels, Percent>(Widget::ConstraintPreset p, Widget* pParent, NormalizedPixels fWidth, Percent fHeight);
template void Widget::addPresetConstraints<Percent, NormalizedPixels>(Widget::ConstraintPreset p, Widget* pParent, Percent fWidth, NormalizedPixels fHeight);
template void Widget::addPresetConstraints<NormalizedPixels, NormalizedPixels>(Widget::ConstraintPreset p, Widget* pParent, NormalizedPixels fWidth, NormalizedPixels fHeight);

template void Widget::addPresetConstraints<NormalizedPixels, Pixels>(Widget::ConstraintPreset p, Widget* pParent, NormalizedPixels fWidth, Pixels fHeight);
template void Widget::addPresetConstraints<Pixels, NormalizedPixels>(Widget::ConstraintPreset p, Widget* pParent, Pixels fWidth, NormalizedPixels fHeight);

/*!
* \brief
* [Widget::isInHierarchy]
*
* \author Matthias.Gruen
* \date 2021/01/05
* [1/5/2021 Matthias.Gruen]
*/
bool Widget::isInHierarchy(Widget* w) noexcept
{
    if (w == this)
        return true;
    for (const auto& info : children())
        if (info.child()->isInHierarchy(w))
            return true;
    return false;
}

}

}
