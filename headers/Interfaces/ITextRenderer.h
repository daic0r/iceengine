//
//  ITextRenderer.h
//  IceCraft
//
//  Created by Matthias Gruen on 17.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ITextRenderer_h
#define ITextRenderer_h

#include <unordered_map>
#include <vector>
#include <string>

namespace Ice {

class GUIText;
class BaseTexture;

namespace GUI {
    class TextLabel;
}

class ITextRenderer {
public:
    virtual BaseTexture* registerFontTexture(const std::string& strName, const std::string& strTextureFile) = 0;
    virtual void render(GUI::TextLabel*) noexcept = 0;

    virtual ~ITextRenderer() = default;
};

}

#endif /* ITextRenderer_h */
