//
//  FramebufferObjectGL.h
//  IceCraft
//
//  Created by Matthias Gruen on 28.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef FramebufferObjectGL_h
#define FramebufferObjectGL_h

#include <SDL2/SDL_opengl.h>
#include <cstdint>

namespace Ice {

class FramebufferObjectGL {
    GLuint m_nFramebuffer{ 0 };
    GLuint m_nTexAttachment{ 0 };
    GLuint m_nDepthAttachment{ 0 };
	GLuint m_nDepthTextureAttachment{ 0 };
    std::uint16_t m_nWidth{ 0 }, m_nHeight{ 0 };
    GLsizei m_nOldWidth{}, m_nOldHeight{};
    GLint m_nLastFBO{};
    
public:
    FramebufferObjectGL() = default;
    FramebufferObjectGL(std::uint16_t w, std::uint16_t h, bool bCreate = true) noexcept;
    FramebufferObjectGL(const FramebufferObjectGL&) = delete;
    FramebufferObjectGL& operator=(const FramebufferObjectGL&) = delete;
    FramebufferObjectGL(FramebufferObjectGL&&) noexcept;
    FramebufferObjectGL& operator=(FramebufferObjectGL&&) noexcept;
    ~FramebufferObjectGL();
    
    bool createTextureAttachment() noexcept;
    bool createDepthAttachment() noexcept;
	bool createDepthTextureAttachment() noexcept;
    
    auto width() const noexcept { return m_nWidth; }
    auto height() const noexcept { return m_nHeight; }
	void setWidth(std::uint16_t w) noexcept { m_nWidth = w; }
	void setHeight(std::uint16_t h) noexcept { m_nHeight = h; }
    auto textureAttachmentId() const noexcept { return m_nTexAttachment; }
    void setTextureAttachmentId(GLuint id) noexcept { m_nTexAttachment = id; }
    auto depthAttachmentId() const noexcept { return m_nDepthAttachment; }
    void setDepthAttachmentId(GLuint id) noexcept { m_nDepthAttachment = id; }
	auto depthTextureAttachmentId() const noexcept { return m_nDepthTextureAttachment; }
	void setDepthTextureAttachmentId(GLuint id) noexcept { m_nDepthTextureAttachment = id; }
    auto lastFBO() const noexcept { return m_nLastFBO; }
    void setLastFBO(GLint nFbo) noexcept { m_nLastFBO = nFbo; }
    auto oldWidth() const noexcept { return m_nOldWidth; }
    void setOldWidth(GLsizei nWidth) noexcept { m_nOldWidth = nWidth; }
    auto oldHeight() const noexcept { return m_nOldHeight; }
    void setOldHeight(GLsizei nHeight) noexcept { m_nOldHeight = nHeight; }
    
	void create() noexcept;
    void bind(GLsizei nOldWidth = 0, GLsizei nOldHeight = 0) noexcept;
    void unbind() noexcept;
    void resize(GLsizei nWidth, GLsizei nHeight) noexcept;

    
    void swap(FramebufferObjectGL&) noexcept;
    friend void swap(FramebufferObjectGL& a, FramebufferObjectGL& b) noexcept {
        a.swap(b);
    }
};


}


#endif /* FramebufferObjectGL_h */
