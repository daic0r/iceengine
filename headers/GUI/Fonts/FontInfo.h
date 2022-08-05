//
//  FontInfo.h
//  IceCraft
//
//  Created by Matthias Gruen on 16.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef FontInfo_h
#define FontInfo_h

#include <map>
#include <string>
#include <tuple>

namespace Ice {

class BaseTexture;

namespace GUI {

struct FontCharacterInfo {
    // ASCII code
    unsigned char id;
    int pixX, pixY;
    // (x,y) position of top left corner of character in the texture (jn pixels)
    float x, y;
    int width, height;
    // width and height in texture coordinates
    float texWidth, texHeight;
    // top left corner position of character relative to the text baseline
    int xoffset, yoffset;
    // how far to move the cursor after the character has been added
    int xadvance;
};

struct FontInfo {
    std::string name;
    struct {
        int top, right, bottom, left;
    } padding;
    int size;
    int lineHeight;
    int baselineHeight;
    bool bold;
    bool italic;
    std::map<unsigned char, FontCharacterInfo> characters;
    BaseTexture *pTexture{ nullptr };
};

}

}

#endif /* FontInfo_h */
