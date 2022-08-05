//
//  TextManager.h
//  IceCraft
//
//  Created by Matthias Gruen on 16.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef FontManager_h
#define FontManager_h

#include <string>
#include <unordered_map>
#include <Interfaces/ITextManager.h>
#include <Interfaces/ITextRenderer.h>

#include "FontInfo.h"
#include <vector>

namespace Ice {

namespace GUI {

class BaseTexture;
class ITextRenderer;

class TextManager : public ITextManager {
    static bool m_bInstanced;
    static inline constexpr int DESIRED_PADDING = 3;
    
    std::unordered_map<std::string, FontInfo> m_mFonts;
    
public:
    TextManager();
    
    void registerFont(const std::string& strName) override;
    const FontInfo& getFont(const std::string& strName) const override;
    
private:
    
    static FontInfo readFontFile(const std::string& strFile);
};

}

}

#endif /* FontManager_h */
