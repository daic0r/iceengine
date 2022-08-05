//
//  TextureGL.h
//  IceCraft
//
//  Created by Matthias Gruen on 07.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef GLTexture_h
#define GLTexture_h


#include <SDL2/SDL_opengl.h>
#include <string>
#include <vector>
//#include <Interfaces/ITexture.h>
#include <System/BaseTexture.h>

namespace Ice {

class TextureGL : public BaseTexture {
    GLuint m_textureId{ 0 };
    
public:
    
    TextureGL();
    TextureGL(const TextureGL&) = delete;
    TextureGL& operator=(const TextureGL&) = delete;
    TextureGL(TextureGL&&);
    TextureGL& operator=(TextureGL&&);
    ~TextureGL();
    
    void loadFromFile(const std::string&) override;
    void loadFromID(GLuint id) noexcept { m_textureId = id; }
    //void initialize();
    void bind(int, unsigned int nTextureUnit = 0) const noexcept;
    void unbind() const noexcept;
    
    inline auto textureId() const noexcept { return m_textureId; }
};

}

#endif /* GLTexture_h */
