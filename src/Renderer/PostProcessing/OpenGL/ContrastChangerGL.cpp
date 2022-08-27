#ifdef RENDERER_OPEN_GL

#include <Renderer/PostProcessing/OpenGL/ContrastChangerGL.h>
#include <ShadersGL/ShaderProgramGL.h>
#include <System/SystemServices.h>
#include <Interfaces/IGraphicsSystem.h>

namespace Ice {

ContrastChangerGL::ContrastChangerGL(GLsizei nWidth, GLsizei nHeight)
    : BasePostProcessorGL{ nWidth, nHeight }
{}

const char* ContrastChangerGL::getFragmentShaderSource() const noexcept {
    return R"(
#version 410

in vec2 texCoord;
out vec4 outColor;

uniform sampler2D tex;

void main() {
    outColor = texture(tex, texCoord).grba;
}
)";

}

}

#endif