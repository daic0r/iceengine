//
//  StackPanel.h
//  IceCraft
//
//  Created by Matthias Gruen on 21.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef StackPanel_h
#define StackPanel_h

#include <GUI/Widget.h>
#include <GUI/Constraints/AlignmentConstraint.h>
#include <GUI/Constraints/DirectSizeConstraint.h>
#include <GUI/Constraints/DynamicConstraint.h>
#include <optional>
#include <System/Tools.h>

namespace Ice {

namespace GUI {

template<OrthogonalAxis Axis>
class StackPanel : public Widget {
    HorizontalAlignment m_defaultHorizAlign{ HorizontalAlignment::CENTER };
    VerticalAlignment m_defaultVertAlign{ VerticalAlignment::MIDDLE };
    std::optional<bool> m_bSizesWithChildren;
    float m_fElementSpacing{};

public:
    StackPanel() {
        setColor(glm::vec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        setPadding(0.0f, 0.0f, 0.0f, 0.0f);
    }

    auto defaultHorizontalAlignment() const noexcept { return m_defaultHorizAlign; }
    void setDefaultHorizontalAlignment(HorizontalAlignment a) noexcept { m_defaultHorizAlign = a; }
    auto defaultVerticalAlignment() const noexcept { return m_defaultVertAlign; }
    void setDefaultVerticalAlignment(VerticalAlignment a) noexcept { m_defaultVertAlign = a; }
    float contentSize() const noexcept {
        float fSize{ 0.0f };
        for (const auto& info : children()) {
            if constexpr(Axis == OrthogonalAxis::VERTICAL)
                fSize += info.child()->height(true); // <-- must be true, otherwise the height
            else
                fSize += info.child()->width(true);
                // (which is dynamically calculated at the bottom of the file with a dynamic
                // constraint as well) will not have been updated yet and we'd use an old value
        }
        return fSize;
    }
    auto elementSpacing() const noexcept { return m_fElementSpacing; }
    void setElementSpacing(float f) noexcept { m_fElementSpacing = f; }
    bool isLayoutWidget() const noexcept override { return true; }

    void addChild(Widget* w, size_t nDrawOrder = 1) override {
        if (children().size() == 0)
            m_bSizesWithChildren = getSizeConstraint(Axis) == nullptr;

        //if (m_bSizesWithChildren.value() && w->getSizeConstraint(Axis) == nullptr) {
        //    throw std::runtime_error("A widget added to a stack panel that calculates its size based on the size of its children must have a size constraint");
        //}

        // Added child is aligned to the left, as wide as the stack panel and will
        // be aligned with the bottom of the preceding element
        w->setParent(this);
        if constexpr(Axis == OrthogonalAxis::VERTICAL) {
            if (auto pHorizAlign = w->getPointConstraint(OrthogonalAxis::HORIZONTAL); pHorizAlign == nullptr)
                w->setPointConstraint(defaultHorizontalAlignment(), std::make_unique<HorizontalAlignmentConstraint>(defaultHorizontalAlignment()));
            else
                pHorizAlign->setParent(this);
        }
        else {
            if (auto pVertAlign = w->getPointConstraint(OrthogonalAxis::VERTICAL); pVertAlign == nullptr)
                w->setPointConstraint(defaultVerticalAlignment(), std::make_unique<VerticalAlignmentConstraint>(defaultVerticalAlignment()));
            else
                pVertAlign->setParent(this);
        }

        // Set position constraint of first child here, honoring the default alignment specified
        if (children().size() == 0) {
            if constexpr (Axis == OrthogonalAxis::VERTICAL)
                w->setPointConstraint(VerticalAlignment::TOP, std::make_unique<DynamicPointConstraint>(&_calcFirstChildPointConstraints)); 
            else {
                w->setPointConstraint(HorizontalAlignment::LEFT, std::make_unique<DynamicPointConstraint>(&_calcFirstChildPointConstraints));
            }
        }
        else {
            if constexpr(Axis == OrthogonalAxis::VERTICAL)
                w->setPointConstraint(VerticalAlignment::TOP, 
                    std::make_unique<DynamicPointConstraint>([this](Widget* pParent) {
                        return pParent->bottom() - elementSpacing();
                    }, true, children().back().child()));
                 //std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::BOTTOM, false, children().back().pChild));
            else
                w->setPointConstraint(HorizontalAlignment::LEFT,
                    std::make_unique<DynamicPointConstraint>([this](Widget* pParent) {
                        return pParent->right() + elementSpacing();
                    }, true, children().back().child()));
                //std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::RIGHT, false, children().back().pChild));
        }
        w->setSizeConstraint(Axis == OrthogonalAxis::VERTICAL ? OrthogonalAxis::HORIZONTAL : OrthogonalAxis::VERTICAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));

        Widget::addChild(w, nDrawOrder);

        auto pSizeConstraint = getSizeConstraint(Axis);
        // If we have no size constraint
        // then we'll automatically size and will grow along with the size of the children...
        if (pSizeConstraint == nullptr) {
            setSizeConstraint(Axis, std::make_unique<DynamicSizeConstraint>([](Widget* w) {
                float fDim = 0.0f;
                for (const auto& info : w->children()) {
                    fDim += info.child()->dimension(Axis);
                }
                return fDim;
            }, true, this));
        } else {
        // ... if we have a fixed size, calculate the child's size based on our size
            if (w->getSizeConstraint(Axis) == nullptr) {
                w->setSizeConstraint(Axis, std::make_unique<DynamicSizeConstraint>([](Widget* w) {
                    auto pStackPanel = static_cast<StackPanel*>(w);
                    return pStackPanel->dimension(Axis) / (float)pStackPanel->children().size();
                    }));
                if (!isConstructing())
                    w->refreshConstraints();
            }
        }
        // Update whole hierarchy once
        if (!isConstructing())
            setConstraintsDirty(true, true); //#WATCH: Set first param to true, will it lead to problems?

//        if (!hasClipFrameFunc()) {
//            setClipFrameFunc([](const Widget* w) {
//                return w->parent() != nullptr ? w->parent()->extents() : w->extents();
//            });
//        }
    }

    void removeChild(Widget* pWidget, bool bMakeRootLevelWidget = false) override {
        if (children().size() < 2) {
            Widget::removeChild(pWidget);
            return;
        }
        auto iter = std::find_if(children().begin(), children().end(), [w=pWidget](const auto& childInfo) {
            return childInfo.child() == w;
        });
        // 3 cases: widget to be removed is 1) first, 2) in the middle, 3) at the end
        if (iter == children().begin()) {
            // Move to next one and give it the constraints of a first child,
            // because after deletion it will be the new first element
            iter = std::next(iter);
            if constexpr (Axis == OrthogonalAxis::VERTICAL) 
                iter->child()->setPointConstraint(VerticalAlignment::TOP, std::make_unique<DynamicPointConstraint>(&_calcFirstChildPointConstraints)); 
            else 
                iter->child()->setPointConstraint(HorizontalAlignment::LEFT, std::make_unique<DynamicPointConstraint>(&_calcFirstChildPointConstraints));
            while (iter != children().end()) {
                iter->child()->setConstraintsDirty(false, true);
                ++iter;
            }
        } else if (iter != std::prev(children().end())) {
            auto pNext = std::next(iter)->child();
            auto pPrev = std::prev(iter)->child();
            auto pConstraint = pNext->getPointConstraint(Axis);
            if (pConstraint != nullptr)
                pConstraint->setParent(pPrev);
            iter = std::next(iter);
            while (iter != children().end()) {
                iter->child()->setConstraintsDirty(false, true);
                ++iter;
            }
        }
        // For case 3) only the below is necessary, so above we check if we are
        // not the last one (which means we're in the middle)
        Widget::removeChild(pWidget, bMakeRootLevelWidget);
        pWidget->setSizeConstraint(Axis, nullptr);
    }

private:
    static float _calcFirstChildPointConstraints(Widget* w) {
        if constexpr(Axis == OrthogonalAxis::VERTICAL) {
            float fRet{ 0.0f };
            auto pStackPanel = static_cast<StackPanel*>(w);
            switch (pStackPanel->defaultVerticalAlignment()) {
            case VerticalAlignment::MIDDLE:
            {
                float fHeight = pStackPanel->contentSize();
                const float fMiddle = (w->bottom(false) + w->top(false)) / 2.0f;
                const float fHeight2 = (fHeight / 2.0f);
                fRet = std::min(fMiddle + fHeight2, w->top(false));
            }
            break;
            case VerticalAlignment::TOP:
                fRet = w->top();
                break;
            case VerticalAlignment::BOTTOM:
            {
                float fHeight = pStackPanel->contentSize();
                fRet = w->bottom() + fHeight;
            }
            break;
            }
            return fRet;
        }
        else
        if constexpr(Axis == OrthogonalAxis::HORIZONTAL) {
            float fRet{ 0.0f };
            auto pStackPanel = static_cast<StackPanel*>(w);
            switch (pStackPanel->defaultHorizontalAlignment()) {
            case HorizontalAlignment::CENTER:
            {
                const float fWidth = pStackPanel->contentSize();
                const float fCenter = (w->left(false) + w->right(false)) / 2.0f;
                const float fWidth2 = (fWidth / 2.0f);
                fRet = std::max(fCenter - fWidth2, w->left(false));
            }
                break;
            case HorizontalAlignment::LEFT:
                fRet = w->left();
                break;
            case HorizontalAlignment::RIGHT:
            {
                float fWidth = pStackPanel->contentSize();
                fRet = w->right() - fWidth;
            }
                break;
            }
            return fRet;
        }
        throw std::runtime_error("Invalid axis");
    }
};

using VerticalStackPanel = StackPanel<OrthogonalAxis::VERTICAL>;
using HorizontalStackPanel = StackPanel<OrthogonalAxis::HORIZONTAL>;

}

}
#endif /* StackPanel_h */
