#ifdef RENDERER_OPEN_GL

#include <Renderer/PostProcessing/OpenGL/OriginalCanvasGL.h>

namespace Ice
{
    OriginalCanvasGL::OriginalCanvasGL(GLsizei nWidth, GLsizei nHeight)
        : BasePostProcessorGL{ nWidth, nHeight, true }
    {}

    const char* OriginalCanvasGL::getFragmentShaderSource() const noexcept {
        return R"(
#version 410

in vec2 texCoord;
uniform sampler2D tex;

layout (location = 0) out vec4 outColor;

void main() {
    outColor = texture(tex, texCoord);
}
)";
    }
} // namespace Ice

#endif