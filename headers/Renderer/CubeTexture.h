//
//  CubeTexture.h
//  IceCraft
//
//  Created by Matthias Gruen on 18.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef CubeTexture_h
#define CubeTexture_h

#include "../Interfaces/ITexture.h"
#include <array>
#include <string_view>

struct SDL_Surface;

namespace Ice {

class CubeTexture : public ITexture {
    static constexpr std::array<std::string_view, 6> m_arFaceNames {
        "right", "left", "top", "bottom", "front", "back"
    };
    
    GLuint m_textureId{ 0 };

public:
    
    CubeTexture();
    ~CubeTexture();
    void loadFromFile(const std::string&) override;
    void bind(GLint, GLuint nTextureUnit = 0) const noexcept override;
    void unbind() const noexcept override;
};

}

#endif /* CubeTexture_h */
