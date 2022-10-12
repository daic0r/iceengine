//
//  ScrollWidget.h
//  IceCraft
//
//  Created by Matthias Gruen on 21.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ScrollWidget_h
#define ScrollWidget_h

#include <IceEngine.h>
#include <GUI/Widget.h>

namespace Ice {

namespace GUI {

template<OrthogonalAxis>
class StackPanel;
class Button;

class ScrollWidget : public Widget {
    static constexpr auto BUTTON_SIZE = 25_px;
    
    float m_fScrollPositionVertical{ 0.0 };
    float m_fScrollPositionHorizontal{ 0.0 };
    //StackPanel<OrthogonalAxis::VERTICAL>* m_pContent{ nullptr };
    Widget* m_pContent{ nullptr };
    Button* m_pScrollBarButtonVertical{ nullptr };
    Button* m_pScrollBarButtonHorizontal{ nullptr };
    Button* m_pTopButton{ nullptr };
    Button* m_pBottomButton{ nullptr };
    Button* m_pLeftButton{ nullptr };
    Button* m_pRightButton{ nullptr };
    Widget* m_pScrollBarContainerVertical{ nullptr };
    Widget* m_pScrollBarContainerHorizontal{ nullptr };
    bool m_bInitialized{ false };
    bool m_bHasVerticalScrollBar{ true };
    bool m_bHasHorizontalScrollBar{ true };
    glm::vec4 m_controlColor;

    mutable float m_fScrollBarHeightFactorVertical{ -1.0f };
    mutable float m_fScrollBarHeightFactorHorizontal{ -1.0f };
    mutable float m_fScrollBarToContentFactorVertical{ -1.0f };
    mutable float m_fScrollBarToContentFactorHorizontal{ -1.0f };

public:
    ScrollWidget();

    bool hasVerticalScrollBar() const noexcept { return m_bHasVerticalScrollBar; }
    void setVerticalScrollBar(bool b) noexcept { m_bHasVerticalScrollBar = b; }

    bool hasHorizontalScrollBar() const noexcept { return m_bHasHorizontalScrollBar; }
    void setHorizontalScrollBar(bool b) noexcept { m_bHasHorizontalScrollBar = b; }

    const auto content() const noexcept { return m_pContent; }
    auto content() noexcept { return m_pContent; }
    void setContent(Widget* pContent) noexcept { m_pContent = pContent; }

    const Button* scrollBarButton(OrthogonalAxis axis) const noexcept { 
        switch (axis) {
        case OrthogonalAxis::VERTICAL:
            return m_pScrollBarButtonVertical;
        case OrthogonalAxis::HORIZONTAL:
            return m_pScrollBarButtonHorizontal;
        default:
            return nullptr;
        }
     }
    Button* scrollBarButton(OrthogonalAxis axis) noexcept {
        return const_cast<Button*>(const_cast<const ScrollWidget*>(this)->scrollBarButton(axis));
    }

    const Button* topButton(OrthogonalAxis axis) const noexcept {
        switch (axis) {
        case OrthogonalAxis::VERTICAL:
            return m_pTopButton;
        case OrthogonalAxis::HORIZONTAL:
            return m_pLeftButton;
        default:
            return nullptr;
        }
    }
    Button* topButton(OrthogonalAxis axis) noexcept {
        return const_cast<Button*>(const_cast<const ScrollWidget*>(this)->topButton(axis));
    }

    const Button* bottomButton(OrthogonalAxis axis) const noexcept { 
        switch (axis) {
        case OrthogonalAxis::VERTICAL:
            return m_pBottomButton;
        case OrthogonalAxis::HORIZONTAL:
            return m_pRightButton;
        default:
            return nullptr;
        }
    }
    Button* bottomButton(OrthogonalAxis axis) noexcept {
        return const_cast<Button*>(const_cast<const ScrollWidget*>(this)->bottomButton(axis));
    }
    
    const Widget* scrollBarContainer(OrthogonalAxis axis) const noexcept {
        switch (axis) {
        case OrthogonalAxis::VERTICAL:
            return m_pScrollBarContainerVertical;
        case OrthogonalAxis::HORIZONTAL:
            return m_pScrollBarContainerHorizontal;
        default:
            return nullptr;
        }
    }
    Widget* scrollBarContainer(OrthogonalAxis axis) noexcept {
        return const_cast<Widget*>(const_cast<const ScrollWidget*>(this)->scrollBarContainer(axis));
    }
    
    float scrollBarHeightFactor(OrthogonalAxis axis, bool bRefresh = false) const noexcept;
    float scrollContainerToContentFactor(OrthogonalAxis axis, bool bRefresh = false) const noexcept;
    auto scrollPosition(OrthogonalAxis axis) const noexcept { 
        switch (axis) {
        case OrthogonalAxis::VERTICAL:
            return m_fScrollPositionVertical;
        case OrthogonalAxis::HORIZONTAL:
            return m_fScrollPositionHorizontal;
        }
        return -1.0f;
    }

    float sizeScrollBarContainerBetweenButtons(OrthogonalAxis) const;
    
    void scroll(OrthogonalAxis axis, float);
    
    void refreshConstraints() override;
    void addChild(Widget*, size_t nDrawOrder = 1) override;
    void render(IWidgetRenderer*) override;
    
private:
    
    void createButtons(OrthogonalAxis) noexcept;
    void initContentWidget();
    void createScrollContainer(OrthogonalAxis);
    void createScrollBarButton(OrthogonalAxis);
    void adjustScrollers();
};

}

}


#endif /* ScrollWidget_h */
