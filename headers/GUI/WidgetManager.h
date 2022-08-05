//
//  WidgetManager.h
//  IceCraft
//
//  Created by Matthias Gruen on 14.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef WidgetManager_h
#define WidgetManager_h

#include <System/Types.h>
#include <System/Extents.h>
#include <unordered_map>
#include <GUI/Widget.h>
#include <GUI/Theme.h>
#include <GUI/TextLabel.h>
#include <GUI/Fonts/TextStyle.h>
#include <Utils/TemplateHelpers.h>

namespace Ice {

namespace GUI {

class WidgetManager {
    friend class Widget;

    WidgetContainer m_mWidgets;
    bool m_bMouseIsOverGUI{ false };
    Widget* m_pFocusedRootWidget{ nullptr };
    Widget* m_pDraggingWidget{ nullptr };
    std::vector<Widget*> m_vRootLevelWidgets;
    Theme m_theme;

public:
    template<typename WidgetType = Widget, bool Contained = false, typename... Args>
    auto createWidgetWithFunc(const std::function<void(WidgetType*)>& preAddChildFunc, Widget* pParent, Args&&... args) noexcept {
        std::unique_ptr<WidgetType> pWidget;
		if constexpr (std::is_base_of_v<TextLabel, WidgetType>) {
			if constexpr(sizeof...(Args) > 0 && std::is_same_v<decltype(getPackElement<sizeof...(Args) - 1>(args...)), bool>)
				pWidget = std::make_unique<WidgetType>(std::forward<Args>(args)..., theme().textStyle());
			else
				pWidget = std::make_unique<WidgetType>(std::forward<Args>(args)..., TextLabel::MultilineDefaultSetting, theme().textStyle());
		}
        else
            pWidget = std::make_unique<WidgetType>(std::forward<Args>(args)...);
        // If theming hasn't been explicitly disabled by setting alpha to -1
        // we use the theme's foreground color
        if (pWidget->foregroundColor().a >= 0.0f)
            pWidget->setForegroundColor(theme().foregroundColor());
        if (!pWidget->isLayoutWidget())
            pWidget->setColor(theme().color());

        auto [iter, succ] = m_mWidgets.emplace(pWidget->handle(), std::move(pWidget));
     
        // During call of the lambda below the parent is not known yet,
        // so be careful when adding constraints, parent must be passed
        // explicitly!
        if (preAddChildFunc)
            preAddChildFunc(static_cast<WidgetType*>(iter->second.get()));
        if (pParent != nullptr)
            pParent->addChild(iter->second.get());
        else
            m_vRootLevelWidgets.push_back(iter->second.get());
        if constexpr(Contained)
            iter->second->setIsTabStoppable(false);
        
        iter->second.get()->onAddedToParent();
        
        return static_cast<WidgetType*>(iter->second.get());
    }

    template<typename WidgetType = Widget, bool Contained = false, typename... Args>
    auto createWidget(Widget* pParent, Args&&... args) noexcept {
        return createWidgetWithFunc<WidgetType, Contained>(std::function<void(WidgetType*)>{nullptr}, pParent, std::forward<Args>(args)...); 
    }
    void deleteWidget(WidgetHandle, int nLevel = 0);
    void relocateWidget(Widget* pWidget, Widget* pNewParent);
    
    const Widget* getWidget(WidgetHandle h) const { return m_mWidgets.at(h).get(); }
    Widget* getWidget(WidgetHandle h) { return m_mWidgets.at(h).get(); }
    
    const auto& widgets() const noexcept { return m_mWidgets; }
    const auto& rootLevelWidgets() const noexcept { return m_vRootLevelWidgets; }
    
    void update(float fDeltaTime);
    bool isMouseOverGUI() const noexcept { return m_bMouseIsOverGUI; }
    
    auto focusedRootWidget() const noexcept { return m_pFocusedRootWidget; }
    auto draggingWidget() const noexcept { return m_pDraggingWidget; }

    void setTheme(const Theme& t) noexcept { m_theme = t; }
    const Theme& theme() const noexcept { return m_theme; }

private:

    void setFocusedRootWidget(Widget* w) noexcept { m_pFocusedRootWidget = w; }
    void setDraggingWidget(Widget* w) noexcept { m_pDraggingWidget = w; }
    Widget* findChildAt(Widget*, float, float) noexcept;
    Widget* walkUpHierarchyAtPointIf(Widget*, float, float, const std::function<bool(Widget*)>& = nullptr) noexcept;
    void processEvents() noexcept;
    static std::pair<float,float> _windowCoordToNDC(int x, int y) noexcept;
    void removeRootLevelWidget(Widget*);
};

}

}


#endif /* WidgetManager_h */
