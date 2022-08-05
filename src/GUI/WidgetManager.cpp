//
//  WidgetManager.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 14.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GUI/WidgetManager.h>
#include <Events/EventQueue.h>
#include <System/SystemServices.h>
#include <iostream>
#include <Interfaces/IGraphicsSystem.h>
#include <Events/MouseMotionEventArgs.h>
#include <Events/MouseButtonEventArgs.h>
#include <Events/WindowResizedEvent.h>
#include <Events/TextInputEventArgs.h>
#include <GUI/ScrollWidget.h>

namespace Ice {

namespace GUI {

void WidgetManager::deleteWidget(WidgetHandle h, int nLevel) {
    auto iter = m_mWidgets.find(h);
    if (iter == m_mWidgets.end())
        throw std::runtime_error("Widget handle not found");
    
    removeRootLevelWidget(iter->second.get());

    for (auto& info : iter->second.get()->children()) {
        deleteWidget(info.child()->handle(), nLevel+1);
    }

    // Only notify parent if we're the widget this was initially called on
    // This widget will be deleted anyway, so no point in the children notifying us
    // that they were removed. Plus this will mess up the loop above, since it will
    // modify the container of children while the loop is running
    if (nLevel == 0) {
        const auto pParent = iter->second->parent();
        if (pParent != nullptr) {
            pParent->removeChild(iter->second.get());
        }
    }

    m_mWidgets.erase(iter);
}

void WidgetManager::relocateWidget(Widget *pWidget, Widget* pNewParent) {
    auto iter = m_mWidgets.find(pWidget->handle());
    if (iter == m_mWidgets.end())
        throw std::runtime_error("Widget handle not found");

    if (pWidget->parent() == pNewParent)
        return;

    Widget* pOldParentX{};
    auto pX = pWidget->getPointConstraint(OrthogonalAxis::HORIZONTAL);
    if (pX != nullptr) {
        pOldParentX = pX->parent();
    }
    Widget* pOldParentY{};
    auto pY = pWidget->getPointConstraint(OrthogonalAxis::VERTICAL);
    if (pY != nullptr) {
        pOldParentY = pY->parent();
    }
    Widget* pOldParentWidth{};
    auto pWidth = pWidget->getSizeConstraint(OrthogonalAxis::HORIZONTAL);
    if (pWidth != nullptr) {
        pOldParentWidth = pWidth->parent();
    }
    Widget* pOldParentHeight{};
    auto pHeight = pWidget->getSizeConstraint(OrthogonalAxis::VERTICAL);
    if (pHeight != nullptr) {
        pOldParentHeight = pHeight->parent();
    }

    auto pOldParent = pWidget->parent();
    if (pOldParent != nullptr)
        pOldParent->removeChild(pWidget);
    else {
        removeRootLevelWidget(pWidget);
    }

    pNewParent->addChild(pWidget);
    if (pOldParentX == pOldParent)
        pX->setParent(pNewParent);
    if (pOldParentY == pOldParent)
        pY->setParent(pNewParent);
    if (pOldParentWidth == pOldParent)
        pWidth->setParent(pNewParent);
    if (pOldParentHeight == pOldParent)
        pHeight->setParent(pNewParent);
    pWidget->setConstraintsDirty(false, true); 
}

/*
 * \name findChildAt
 *
 * \param pStart Start widget
 * \param x x position in screen NDC
 * \param y y position in screen NDC
 *
 * \description Finds the bottommost child of pStart that lies at (x,y)
 *
 * \date 03.01.2021
 */
Widget* WidgetManager::findChildAt(Widget* pStart, float x, float y) noexcept {
    if (pStart->visible() && !pStart->hasChildren() && pStart->isInsideWidget(x, y))
        return pStart;
    
    Widget* pRet{ nullptr };
    for (const auto& info : pStart->children()) {
        auto pWidget = info.child();
        if (pWidget->visible() && pWidget->isInsideWidget(x, y)) {
            pRet = findChildAt(pWidget, x, y);
            if (pRet != nullptr)
                break;
        }
    }
    return pRet != nullptr ? pRet : pStart;
}

/*
 * \name walkUpHierarchyAtPointIf
 *
 * \param pStart Widget where we start walking up the hierarchy
 * \param x x position in screen NDC
 * \param y y position in screen NDC
 * \param AdditionalConditionsPred Predicate to check in addition to visibility and if (x,y) is inside the widget
 *
 * \description Walks up the hierarchy starting from a widget, checking if a given point (x,y) is inside the widget
 *              and if the widget is visible. In addition, the return value of the predicate is checked before
 *              walking up.
 *
 * \date 03.01.2021
 */
Widget* WidgetManager::walkUpHierarchyAtPointIf(Widget* pStart, float x, float y, const std::function<bool(Widget*)>& AdditionalConditionsPred /* = nullptr*/) noexcept {
    Widget* pTopLevelWidget{ nullptr };
    pTopLevelWidget = pStart;
    while (true) {
        auto pParent = pTopLevelWidget->parent();
        if (pParent != nullptr && pParent->visible() && pParent->isInsideWidget(x, y)) {
            const bool bAdditionalCondition = AdditionalConditionsPred != nullptr ? AdditionalConditionsPred(pParent) : true;
            if (bAdditionalCondition)
                pTopLevelWidget = pParent;
        }
        else {
            break;
        }
    }
    return pTopLevelWidget;
}

void WidgetManager::processEvents() noexcept {
    while (auto pEvent = systemServices.getEventQueue()->peekInternalEvent()) {
        switch (pEvent->id()) {
            case EventId::MOUSE_MOTION_EVENT:
            {
                m_bMouseIsOverGUI = false;
                const auto& args = std::any_cast<const MouseMotionEventArgs&>(pEvent->args());

                Widget* pFoundWidget = draggingWidget() != nullptr ? draggingWidget() : nullptr;
                const auto [x, y] = _windowCoordToNDC(args.posX(), args.posY());
                if (pFoundWidget == nullptr) {
                    for (auto pWidgetPtr : rootLevelWidgets()) {
                        // The lambda is for the case that the mouse is dragged while
                        // a mouse button is pressed. In that case we don't want to trigger a mouse leave
                        // event but rather allow the dragging to be continued until the mouse button is
                        // released
                        if (pFoundWidget == nullptr) {
                            const bool bDragging = pWidgetPtr->isMouseOver() && (args.buttonState() & (std::uint8_t)MouseButton::LEFT);
                            if (pWidgetPtr->visible() && (pWidgetPtr->isInsideWidget(x, y) || bDragging)) {
                                pFoundWidget = findChildAt(pWidgetPtr, x, y);
                                if (bDragging)
                                    setDraggingWidget(pFoundWidget);
                            }
                        }

                        // m_mWidgets is sorted by handle, so we should get parent widgets before we get their children (right??)
                        // Below we check if we have left this widget, and if so, walk up the hierarchy
                        // while the same applies to the parent of the currently looked at widget in the loop (pWidgetPtr)
                        // We then call onMouseLeave on the last widget obtained that way (the "parentest").
                        // The widget which will then propagate to its children (parent leave means child leave)
                        // Therefore the if-condition below should only return true once in this loop for a given hierarchy,
                        // because the next time we encounter a widget from this hierarchy, isMouseOver() will already be false and we won't
                        // enter the block below
                        pWidgetPtr->apply([x = x, y = y, this](Widget* w) {
                            if (w->isMouseOver() && w->visible() && !w->isInsideWidget(x, y)) {
                                w = walkUpHierarchyAtPointIf(w, x, y, [](Widget* pParent) { return pParent->isMouseOver(); });
                                bool bHandled{ false };
                                w->onMouseLeave(bHandled);
                            }
                            return false;
                        });
                    }
                    if (pFoundWidget == nullptr)
                        break;
                    m_bMouseIsOverGUI = true;
                        
                    // Here we have the "childest" node in the hierarchy, call onMouseEnter on that
                    // onMouseEnter will propagate to the parent (child enter means parent enter)
                    if (!pFoundWidget->isMouseOver()) {
                        bool bHandled{ false };
                        pFoundWidget->onMouseEnter(bHandled);
                    }
                }

                bool bHandled{ false };
                // x,y here are in NDC
                pFoundWidget->onMouseOver(x, y, systemServices.getGraphicsSystem()->getNormalizedPixelsHorizontal(args.offsetX()), systemServices.getGraphicsSystem()->getNormalizedPixelsVertical(args.offsetY()), args.buttonState(), bHandled);
                   
            }
                break;
            case EventId::MOUSE_BUTTON_EVENT:
            {
                const auto& args = std::any_cast<const MouseButtonEventArgs&>(pEvent->args());
                if (!args.pressed() && draggingWidget() != nullptr)
                    setDraggingWidget(nullptr);
                
                auto [x, y] = _windowCoordToNDC(args.posX(), args.posY());
                Widget* pFoundWidget{ nullptr };
                for (auto pWidgetPtr : rootLevelWidgets()) {
                    pWidgetPtr->apply([&pFoundWidget, x = x, y = y, this](Widget* w) {
                        if (pFoundWidget == nullptr && w->visible() && w->isInsideWidget(x, y)) {
                            pFoundWidget = findChildAt(w, x, y);
                            if (pFoundWidget != nullptr)
                                return true;
                        }
                        return false;
                    });
                }
                if (pFoundWidget == nullptr)
                    break;

                // Switch focused root parent if necessary
                auto pRootParent = pFoundWidget->rootParent();
                if (pRootParent != focusedRootWidget()) {
                    if (focusedRootWidget() != nullptr)
                        focusedRootWidget()->onFocusLost();
                    setFocusedRootWidget(pRootParent);
                }
				// Tell all the children of the parent at the root of the hierarchy
                // that they have lost focus, except for the branch that the currently
                // clicked widget is in, of course
				for (const auto& info : pRootParent->children()) {
					if (!info.child()->isInHierarchy(pFoundWidget) && info.child()->hasFocus()) {
						info.child()->onFocusLost();
					}
				}
                if (!pFoundWidget->hasFocus()) {
                    pFoundWidget->onFocusReceived();
                }

                bool bHandled{ false };
                pFoundWidget->onMouseButton(args.button(), args.pressed(), bHandled);
            }
            break;
            case EventId::WINDOW_RESIZED_EVENT:
//                 for (auto& [h, pWidget] : m_mWidgets) {
//                     if (pWidget->parent() == nullptr) {
//                         auto pArgs = static_cast<WindowResizedEventArgs*>(pEvent->args());
//                         pWidget->onWindowResized(args.width(), args.height());
//                     }
//                 }
            {
                const auto& args = std::any_cast<const WindowResizedEventArgs&>(pEvent->args());
                for (auto pWidgetPtr : rootLevelWidgets()) {
                    pWidgetPtr->onWindowResized(args.width(), args.height());
                }
            }
                break;
            case EventId::TEXT_INPUT_EVENT:
                if (focusedRootWidget() == nullptr)
                    break;
                Widget::sTextInputInfo ti;
                auto& args = std::any_cast<TextInputEventArgs&>(pEvent->args());
                ti.ch = args.character();
                ti.bBackspace = args.isBackspace();
                ti.bEscape = args.isEscape();
                ti.bEnter = args.isEnter();
                focusedRootWidget()->apply([&ti](Widget* w) {
                    if (w->isTabStoppable() && w->hasFocus()) {
                        w->onTextInput(ti);
                        return true;
                    }
                    return false;
                });
                break;
        }
    }
} 

void WidgetManager::update(float fDeltaTime) {
    static std::vector<decltype(Widget::m_vAnimations)::iterator> vDelIters;
    for (auto& [h, pWidget] : m_mWidgets) {
        vDelIters.clear();
        auto& animations = pWidget->animations();
        for (auto iter = animations.begin(); iter != animations.end(); ++iter) {
            auto& aniFunc = *iter;
            bool bDone = aniFunc(pWidget.get(), fDeltaTime);
            if (bDone) {
                vDelIters.push_back(iter);
            }
        }
        for (auto iter : vDelIters)
            animations.erase(iter);
    }
    processEvents();
}

std::pair<float,float> WidgetManager::_windowCoordToNDC(int x, int y) noexcept {
    static const auto pGraphics = systemServices.getGraphicsSystem();
    return { pGraphics->getNormalizedPixelsHorizontal(x) - 1.0f, 1.0f - pGraphics->getNormalizedPixelsVertical(y) };
}

/*!
* \brief
* [WidgetManager::removeRootLevelWidget]
*
* \author Matthias.Gruen
* \date 2021/01/07
* [1/7/2021 Matthias.Gruen]
*/
void WidgetManager::removeRootLevelWidget(Widget* w)
{
    if  (auto iter = std::find(m_vRootLevelWidgets.begin(), m_vRootLevelWidgets.end(), w); iter != m_vRootLevelWidgets.end())
        m_vRootLevelWidgets.erase(iter);

    if (auto rootWidgetIter = std::find(m_vRootLevelWidgets.begin(), m_vRootLevelWidgets.end(), w); rootWidgetIter != m_vRootLevelWidgets.end()) {
        m_vRootLevelWidgets.erase(rootWidgetIter);
        if (w == m_pFocusedRootWidget) {
            if (m_vRootLevelWidgets.size() > 0)
                m_pFocusedRootWidget = m_vRootLevelWidgets.front();
            else
                m_pFocusedRootWidget = nullptr;
        }
    }
}

}

}

