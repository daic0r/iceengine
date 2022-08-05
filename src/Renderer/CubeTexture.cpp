//
//  CubeTexture.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 18.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GL/glew.h>
#include <Renderer/CubeTexture.h>
#include <SDL2_image/SDL_image.h>
#include <string>
#include <iostream>
#include <Utils/SDLTools.h>

namespace Ice {

CubeTexture::CubeTexture()  {
}

CubeTexture::~CubeTexture() {
    glDeleteTextures(1, &m_textureId);
}

void CubeTexture::loadFromFile(const std::string& prefix) {
    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureId);
    
    for (int i = GL_TEXTURE_CUBE_MAP_POSITIVE_X; i < GL_TEXTURE_CUBE_MAP_NEGATIVE_Z + 1; ++i) {
        std::string strFilename{ m_arFaceNames[i - GL_TEXTURE_CUBE_MAP_POSITIVE_X] };
        strFilename = prefix + strFilename + ".png"; 
        auto [img, bpp] = SDLTools::readFileToSurface(strFilename, false);
        int nFormat = bpp == 32 ? GL_RGBA : GL_RGB;
        glTexImage2D(
            i,
            0, GL_RGBA, img->w, img->h, 0, nFormat, GL_UNSIGNED_BYTE, img->pixels
        );
        GLenum err;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            std::cout << std::hex << err << std::endl;
        }
        SDL_FreeSurface(img);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void CubeTexture::bind(GLint location, GLuint nTextureUnit) const noexcept {
    //glUniform1i(location, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureId);
}

void CubeTexture::unbind() const noexcept {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

}
