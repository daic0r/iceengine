#ifndef OriginalCanvasGL_h
#define OriginalCanvasGL_h

#ifdef RENDERER_OPEN_GL

#include <Renderer/PostProcessing/OpenGL/BasePostProcessorGL.h>

namespace Ice
{
    class OriginalCanvasGL : public BasePostProcessorGL {
        friend class PostProcessingPipeline;

    public:
        OriginalCanvasGL(GLsizei nWidth = -1, GLsizei nHeight = -1);

    private:
        const char* getFragmentShaderSource() const noexcept override;
    };
    
} // namespace Ice

#endif

#endif