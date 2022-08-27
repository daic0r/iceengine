#ifndef BasePostProcessorGL_h
#define BasePostProcessorGL_h

#ifdef RENDERER_OPEN_GL

#include <Renderer/OpenGL/FramebufferObjectGL.h>
#include <Renderer/OpenGL/RenderObjectGL.h>
#include <ShadersGL/ShaderProgramGL.h>
#include <Interfaces/IPostProcessingEffect.h>

namespace Ice {
    
class BasePostProcessorGL : public IPostProcessingEffect {
public:
    BasePostProcessorGL(GLsizei nWidth = -1, GLsizei nHeight = -1, bool bDepthAttachment = false);

    void initialize();
    void bind();
    void postProcess(IPostProcessingEffect* pPrevious) override;
    void unbind();
    void resize(int nWidth, int nHeight) override;
    bool hasStaticSize() const noexcept { return m_bStaticSize; }
    void setStaticSize(bool b) noexcept { m_bStaticSize = b; }

    GLuint textureId() const noexcept;

protected:
    static inline bool bStaticInit{};
    static RenderObjectGL m_quad;

    FramebufferObjectGL m_fbo;
    ShaderProgramGL m_shader;
    bool m_bStaticSize{};

    static const char* getVertexShaderSource() noexcept;

private:
    virtual const char* getFragmentShaderSource() const noexcept = 0;
};

} // namespace Ice 

#endif

#endif