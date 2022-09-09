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
    GLsizei m_nWidth{ -1 }, m_nHeight{ -1 };
    GLsizei m_nOldWidth{ -1 }, m_nOldHeight{ -1 };
    GLint m_nLastFBO{};
    
public:
    FramebufferObjectGL() = default;
    FramebufferObjectGL(GLsizei w, GLsizei h, bool bCreate = true) noexcept;
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
	void setWidth(GLsizei w) noexcept { m_nWidth = w; }
	void setHeight(GLsizei h) noexcept { m_nHeight = h; }
    auto fboId() const noexcept { return m_nFramebuffer; }
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
    void bind(GLsizei nOldWidth = -1, GLsizei nOldHeight = -1) noexcept;
    void unbind() noexcept;
    void resize(GLsizei nWidth, GLsizei nHeight) noexcept;

    
    void swap(FramebufferObjectGL&) noexcept;
    friend void swap(FramebufferObjectGL& a, FramebufferObjectGL& b) noexcept {
        a.swap(b);
    }

private:
    void cleanup();
};


}


#endif /* FramebufferObjectGL_h */
