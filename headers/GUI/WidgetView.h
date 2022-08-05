//
//  WidgetView.h
//  IceCraft
//
//  Created by Matthias Gruen on 30.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef WidgetView_h
#define WidgetView_h

#include <GUI/Widget.h>

namespace Ice {

class IWidgetRenderer;

namespace GUI {

/////////////////////////////////////////////////////
// WidgetView
/////////////////////////////////////////////////////
//
// Provides a view on another widget.
// This means that a widget can be displayed somewhere else in addition
// to its actual position by means of this widget type.
// This is used in the DropDown widget to display the currently selected
// list item.
/////////////////////////////////////////////////////
class WidgetView : public Widget {
    Widget* m_pWidget{ nullptr };
    
    void swapConstraints() noexcept;
    void refreshConstraints(Widget*);
public:
    WidgetView() = default;
    WidgetView(Widget* w) : m_pWidget{ w } { setGraphicsDirty(true); }
    auto widget() const noexcept { return m_pWidget; }
    void setWidget(Widget *w) noexcept { m_pWidget = w; setGraphicsDirty(true); }
    
    void render(IWidgetRenderer*) override;
};

}


}


#endif /* WidgetView_h */
