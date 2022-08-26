#ifndef ContrastChangerGL_h
#define ContrastChangerGL_h

#include <Renderer/OpenGL/FramebufferObjectGL.h>
#include <Renderer/PostProcessing/OpenGL/BasePostProcessorGL.h>

namespace Ice {

class ContrastChangerGL : public BasePostProcessorGL {
public:
    ContrastChangerGL(GLsizei nWidth = -1, GLsizei nHeight = -1);

private:
    const char* getFragmentShaderSource() noexcept override;
};

}

#endif
