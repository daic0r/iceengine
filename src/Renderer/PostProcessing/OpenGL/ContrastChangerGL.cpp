#ifdef RENDERER_OPEN_GL

#include <Renderer/PostProcessing/OpenGL/ContrastChangerGL.h>
#include <ShadersGL/ShaderProgramGL.h>
#include <System/SystemServices.h>
#include <Interfaces/IGraphicsSystem.h>

namespace Ice {

void ContrastChangerGL::setContrast(float f) noexcept {

    if (m_nContrastUniformId < 0) [[unlikely]]
        m_nContrastUniformId = m_shader.getUniformLocation("contrast");

    m_shader.use();
    m_shader.loadFloat(m_nContrastUniformId, f);
    m_shader.unuse();
}

const char* ContrastChangerGL::getFragmentShaderSource() const noexcept {
    return R"(
#version 410

in vec2 texCoord;
out vec4 outColor;

uniform sampler2D tex;
uniform float contrast;

void main() {
    outColor = texture(tex, texCoord);
    outColor.rgb = (outColor.rgb - 0.5) * (1 + contrast) + 0.5;
}
)";

}

}

#endif