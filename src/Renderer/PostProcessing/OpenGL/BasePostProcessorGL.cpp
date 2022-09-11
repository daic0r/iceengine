#ifdef RENDERER_OPEN_GL

#include <GL/glew.h>
#include <Renderer/PostProcessing/OpenGL/BasePostProcessorGL.h>
#include <ShadersGL/ShaderProgramGL.h>
#include <System/SystemServices.h>
#include <Interfaces/IGraphicsSystem.h>
#include <vector>
#include <Renderer/OpenGL/RenderToolsGL.h>

namespace Ice {

static const std::vector<glm::vec2> vQuad{ 
    glm::vec2{ -1.0f, 1.0f },
    glm::vec2{ -1.0f, -1.0f },
    glm::vec2{ 1.0f, 1.0f },
    glm::vec2{ 1.0f, -1.0 }
};

RenderObjectGL BasePostProcessorGL::m_quad;

BasePostProcessorGL::BasePostProcessorGL(GLsizei nWidth, GLsizei nHeight, bool bDepthAttachment)
    : m_fbo{ nWidth < 0 ? systemServices.getGraphicsSystem()->displayWidth() : nWidth, nHeight < 0 ? systemServices.getGraphicsSystem()->displayHeight() : nHeight }
{
    if (!bStaticInit) [[unlikely]] {
        m_quad = RenderToolsGL::loadVerticesToVAO(vQuad);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);

        bStaticInit = true;
    }

    m_fbo.createTextureAttachment();
    if (bDepthAttachment)
        m_fbo.createDepthAttachment();
    m_fbo.unbind();
}

void BasePostProcessorGL::initialize() {
    m_shader.fromSource(getVertexShaderSource(), getFragmentShaderSource());
    m_shader.use();
    m_shader.loadInt(m_shader.getUniformLocation("tex"), 0); //texture unit 0
    m_shader.unuse();
}

void BasePostProcessorGL::postProcess(IPostProcessingEffect* pPrevious) {
    glClear (GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    // Bind quad VAO
    glBindVertexArray(m_quad.vao());
    // Activate the shader
    m_shader.use();
    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, static_cast<BasePostProcessorGL*>(pPrevious)->textureId());
    // Render the quad
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    // Cleanup
    glBindTexture(GL_TEXTURE_2D, 0);
    m_shader.unuse();
    glBindVertexArray(0);

    return;
}

void BasePostProcessorGL::bind() {
    // Rationale: we're in a postprocessing pipeline
    // --> the viewport size before an effect is bound will be that of the effect
    // (we're applying one effect after another to the output of the previous effect)
    // therefore, save our size as the old size that will be restored upon unbinding
    m_fbo.bind();
}

void BasePostProcessorGL::unbind() {
    m_fbo.unbind();
}

void BasePostProcessorGL::resize(int nWidth, int nHeight) {
    if (hasStaticSize())
        return;
    m_fbo.resize(nWidth, nHeight);
}

GLuint BasePostProcessorGL::textureId() const noexcept {
    return m_fbo.textureAttachmentId();
}

const char* BasePostProcessorGL::getVertexShaderSource() noexcept {
    return R"(
#version 410

layout(location = 0) in vec2 position;

out vec2 texCoord;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    texCoord = position * 0.5 + 0.5;
}
)";
}




}

#endif