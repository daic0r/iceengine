//
//  Texture.h
//  IceCraft
//
//  Created by Matthias Gruen on 07.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef GLTexture_h
#define GLTexture_h


#include <string>
#include <vector>

class Texture {
   
    GLuint m_textureId;
    std::vector<GLfloat> m_vTexCoords;
    GLuint m_locAttrTexCoord; // shader location for tex coord attrib
    GLuint m_locUniTexture; // shader location for texture uniform
    GLuint m_bufTexCoords;
    GLuint m_programId;
    
public:
    
    Texture(unsigned int programId);
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&&);
    Texture& operator=(Texture&&);
    ~Texture();
    
    void loadFromFile(const std::string&);
    void setTexCoords(const std::vector<GLfloat>&);
    void initialize();
    void bind();
    void unbind();
    
    auto id() const noexcept { return m_textureId; }
    
};


#endif /* GLTexture_h */
