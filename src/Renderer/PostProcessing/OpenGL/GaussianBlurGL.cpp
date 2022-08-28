#ifdef RENDERER_OPEN_GL

#include <Renderer/PostProcessing/OpenGL/GaussianBlurGL.h>

namespace Ice
{
    void OneDimensionalGaussianBlurGL::initialize() {
        BasePostProcessorGL::initialize();

        if (m_nKernelSizeUniformId < 0) {
            m_nKernelSizeUniformId = m_shader.getUniformLocation("kernelSize");
        }
        setKernelSize(m_nKernelSize);
        if (m_nTexelDimensionUniformId < 0) {
            m_nTexelDimensionUniformId = m_shader.getUniformLocation("texelDimension");
        }
        setTexelDimension();
    }

    void OneDimensionalGaussianBlurGL::setKernelSize(std::size_t n) noexcept {
        m_shader.use();
        m_shader.loadInt(m_nKernelSizeUniformId, n);
        m_shader.unuse();
    }

    void OneDimensionalGaussianBlurGL::resize(int nDimension, int nHeight) {
        BasePostProcessorGL::resize(nDimension, nHeight);
        setTexelDimension();
    }

    ///////////////////////////////////////////////////////////////////////////

    void HorizontalGaussianBlurGL::setKernelSize(std::size_t n) noexcept {
        OneDimensionalGaussianBlurGL::setKernelSize(n);
    }

    void HorizontalGaussianBlurGL::setTexelDimension() noexcept {
        m_shader.use();
        m_shader.loadFloat(m_nTexelDimensionUniformId, 1.0f / static_cast<float>(m_fbo.width()));
        m_shader.unuse();
    }

    const char* HorizontalGaussianBlurGL::getFragmentShaderSource() const noexcept {
        return R"(
#version 410

in vec2 texCoord;
out vec4 outColor;

uniform sampler2D tex;
uniform int kernelSize;
uniform float texelDimension;

void main() {
    float u = texCoord.x - (kernelSize / 2) * texelDimension;
    outColor = vec4(0.0);
    for (int i = 0; i < kernelSize; ++i) {
        outColor += texture(tex, vec2(min(1.0, max(0.0, u + (i * texelDimension))), texCoord.y));
    }
    outColor /= kernelSize;
}
)";
    }

    ///////////////////////////////////////////////////////////////////////////

    void VerticalGaussianBlurGL::setKernelSize(std::size_t n) noexcept {
        OneDimensionalGaussianBlurGL::setKernelSize(n);
    }

    void VerticalGaussianBlurGL::setTexelDimension() noexcept {
        m_shader.use();
        m_shader.loadFloat(m_nTexelDimensionUniformId, 1.0f / static_cast<float>(m_fbo.height()));
        m_shader.unuse();
    }

    const char* VerticalGaussianBlurGL::getFragmentShaderSource() const noexcept {
        return R"(
#version 410

in vec2 texCoord;
out vec4 outColor;

uniform sampler2D tex;
uniform int kernelSize;
uniform float texelDimension;

void main() {
    float v = texCoord.y - (kernelSize / 2) * texelDimension;
    outColor = vec4(0.0);
    for (int i = 0; i < kernelSize; ++i) {
        outColor += texture(tex, vec2(texCoord.x, min(1.0, max(0.0, v + (i * texelDimension)))));
    }
    outColor /= kernelSize;
}
)";
    }

    ///////////////////////////////////////////////////////////////////////////

    // Attempt at unifying this into a single filter
    // Problem: isn't a BasePostProcessorGL and therefore doesn't have a texture of its own
/*
    GaussianBlurGL::GaussianBlurGL(GLsizei nWidth, GLsizei nHeight)
        : m_horBlur{ nWidth, nHeight },
        m_verBlur{ nWidth, nHeight }
    {}

    void GaussianBlurGL::bind() {
        m_horBlur.bind();
    }

    void GaussianBlurGL::postProcess(IPostProcessingEffect* pPrevious) {
        m_horBlur.postProcess(pPrevious);
        m_verBlur.bind();
        m_verBlur.postProcess(&m_horBlur);
    }

    void GaussianBlurGL::unbind() {
        m_verBlur.unbind();
    }

    void GaussianBlurGL::resize(int nWidth, int nHeight) {
        m_horBlur.resize(nWidth, nHeight);
        m_verBlur.resize(nWidth, nHeight);
    }

    bool GaussianBlurGL::hasStaticSize() const noexcept {
        return m_horBlur.hasStaticSize() && m_verBlur.hasStaticSize();
    }

    void GaussianBlurGL::initialize() {
        m_horBlur.initialize();
        m_verBlur.initialize();
    }

    void GaussianBlurGL::setKernelSize(std::size_t n) noexcept {
        m_horBlur.setKernelSize(n);
        m_verBlur.setKernelSize(n);
    }
    */
} // namespace Ice

#endif