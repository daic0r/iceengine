#ifndef BasePostProcessorGL_h
#define BasePostProcessorGL_h

#include <Renderer/OpenGL/FramebufferObjectGL.h>
#include <Renderer/OpenGL/RenderObjectGL.h>
#include <ShadersGL/ShaderProgramGL.h>

namespace Ice {
    
class BasePostProcessorGL {
public:
    BasePostProcessorGL(GLsizei nWidth = -1, GLsizei nHeight = -1);

    GLuint postProcess(GLuint nTextureID);

protected:
    static inline bool bStaticInit{};
    static RenderObjectGL m_quad;
    FramebufferObjectGL m_fbo;
    ShaderProgramGL m_shader;
    int m_nTexUniformID{ -1 };

    static const char* getVertexShaderSource() noexcept;

private:
    virtual const char* getFragmentShaderSource() noexcept = 0;
};

} // namespace Ice 


#endif