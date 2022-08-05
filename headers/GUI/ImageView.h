//
//  ImageView.h
//  IceCraft
//
//  Created by Matthias Gruen on 23.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ImageView_h
#define ImageView_h

#include <GUI/Widget.h>
#include <System/Image.h>
#include <glm/vec2.hpp>

namespace Ice {

namespace GUI {

class ImageView : public Widget {
public:
    enum class ContentMode {
        FILL,
        SCALE_TO_FIT,
        FIT_HORIZONTALLY,
        FIT_VERTICALLY,
        ORIGINAL
    };
    
    ImageView() = default;
    ImageView(const std::shared_ptr<Image>& pImg, bool bAddSizeConstraints = true);
    ImageView(const std::string&, bool bAddSizeConstraints = true);
    
    bool themable() const noexcept;
    void setThemable(bool b) noexcept;
    
    void setImage(const std::shared_ptr<Image>& pImg) noexcept { m_pImage = pImg; }
    Image* image() const noexcept { return m_pImage.get(); }
    std::pair<glm::vec2, glm::vec2> texCoord() const noexcept;
    
    void setContentMode(ContentMode m) noexcept { m_contentMode = m; }
    ContentMode contentMode() const noexcept { return m_contentMode; }
    
    void render(IWidgetRenderer*) override;
    
private:
    std::shared_ptr<Image> m_pImage;
    ContentMode m_contentMode{ ContentMode::ORIGINAL };

    void _init(bool bAddSizeConstraints);
};

}

}

#endif /* ImageView_h */
