#ifndef ContrastChangerGL_h
#define ContrastChangerGL_h

#ifdef RENDERER_OPEN_GL

#include <Renderer/OpenGL/FramebufferObjectGL.h>
#include <Renderer/PostProcessing/OpenGL/BasePostProcessorGL.h>
#include <Interfaces/IContrastChanger.h>

namespace Ice {

class ContrastChangerGL : public BasePostProcessorGL, public IContrastChanger {
    float m_fContrast{};
    GLint m_nContrastUniformId{ -1 };

public:
    ContrastChangerGL(GLsizei nWidth = -1, GLsizei nHeight = -1);
    void setContrast(float f) noexcept override;

private:
    const char* getFragmentShaderSource() const noexcept override;
};

}

#endif

#endif
