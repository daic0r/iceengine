//
//  IWidgetRenderer.h
//  IceCraft
//
//  Created by Matthias Gruen on 17.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef IWidgetRenderer_h
#define IWidgetRenderer_h

#include <GUI/Widget.h>
#include <unordered_map>

namespace Ice {

struct RenderEnvironment;
namespace GUI {
    class Button;
    class TextLabel;
    class ImageView;
    class ModelPreview;
    class ScrollWidget;
}

class IWidgetRenderer {
public:
    virtual void render(const RenderEnvironment&, const std::vector<GUI::Widget*>&) noexcept = 0;
    
    // render methods for subclasses
    virtual void renderWidget(const GUI::Widget*) = 0;
    virtual void renderWidget(const GUI::Button*) = 0;
    virtual void renderWidget(GUI::TextLabel*) = 0;
    virtual void renderWidget(const GUI::ImageView*) = 0;
    virtual void renderWidget(const GUI::ModelPreview*) = 0;
    virtual void renderWidget(const GUI::ScrollWidget*) = 0;

    virtual ~IWidgetRenderer() {}
};

}


#endif /* IWidgetRenderer_h */
