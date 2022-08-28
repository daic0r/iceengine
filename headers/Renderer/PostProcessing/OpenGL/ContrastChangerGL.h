#ifndef ContrastChangerGL_h
#define ContrastChangerGL_h

#ifdef RENDERER_OPEN_GL

#include <Renderer/OpenGL/FramebufferObjectGL.h>
#include <Renderer/PostProcessing/OpenGL/BasePostProcessorGL.h>
#include <Interfaces/PostProcessing/IContrastChanger.h>

namespace Ice {

class ContrastChangerGL : public BasePostProcessorGL, public IContrastChanger {
    float m_fContrast{};
    GLint m_nContrastUniformId{ -1 };

public:
    using BasePostProcessorGL::BasePostProcessorGL;

    void setContrast(float f) noexcept override;

private:
    const char* getFragmentShaderSource() const noexcept override;
};

}

#endif

#endif
