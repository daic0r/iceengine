//
//  ITextManager.h
//  IceCraft
//
//  Created by Matthias Gruen on 16.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ITextManager_h
#define ITextManager_h

#include <string>

namespace Ice {

namespace GUI {

struct FontInfo;

class ITextManager {
public:
    virtual void registerFont(const std::string& strName) = 0;
    virtual const GUI::FontInfo& getFont(const std::string& strName) const = 0;
    virtual ~ITextManager() = default;
};

}

}

#endif /* ITextManager_h */
