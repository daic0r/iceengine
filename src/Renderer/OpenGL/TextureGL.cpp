//
//  TextureGL.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 07.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <glad/glad.h>
#include <Renderer/OpenGL/TextureGL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <iostream>
#include <utility>
#include <Utils/SDLTools.h>

namespace Ice {

TextureGL::TextureGL() {
}

TextureGL::TextureGL(TextureGL&& other) {
    *this = std::move(other);
}

TextureGL& TextureGL::operator=(TextureGL&& other) {
    m_textureId = std::exchange(other.m_textureId, 0);
    return *this;
}

TextureGL::~TextureGL() {
    if (m_textureId != 0)
        glDeleteTextures(1, &m_textureId);
}

void TextureGL::loadFromFile(const std::string &filename) {
    auto [converted_surface, bpp] = SDLTools::readFileToSurface(filename);
    int nFormat = bpp == 32 ? GL_RGBA : GL_RGB;
    
    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );
//    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -.4);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//

	GLint filter{ 0 };
	switch (minMagFilter()) {
	case MinMagFilter::LINEAR:
		filter = GL_LINEAR;
		break;
	case MinMagFilter::NEAREST:
		filter = GL_NEAREST;
		break;
	default:
		throw std::runtime_error("Invalid MinMagFilter value");
	}
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    if (bpp % 4 != 0)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D (GL_TEXTURE_2D, 0, nFormat, converted_surface->w, converted_surface->h, 0, nFormat, GL_UNSIGNED_BYTE, converted_surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D); //Generate mipmaps now!!!
    if (bpp % 4 != 0)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    setWidth(converted_surface->w);
    setHeight(converted_surface->h);
    setTexelWidth(1.0f / converted_surface->w);
    setTexelHeight(1.0f / converted_surface->h);
    
    SDL_FreeSurface(converted_surface);
    
    unbind();
}

void TextureGL::bind(GLint location, GLuint nTextureUnit /* = 0 */) const noexcept {
    //glUniform1i(location, nTextureUnit);
    glActiveTexture(GL_TEXTURE0 + nTextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
}

void TextureGL::unbind() const noexcept {
    glBindTexture(GL_TEXTURE_2D, 0);
}

}
