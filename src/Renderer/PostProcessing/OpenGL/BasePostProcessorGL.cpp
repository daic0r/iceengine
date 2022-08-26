#include <GL/glew.h>
#include <Renderer/PostProcessing/OpenGL/BasePostProcessorGL.h>
#include <ShadersGL/ShaderProgramGL.h>
#include <System/SystemServices.h>
#include <Interfaces/IGraphicsSystem.h>
#include <vector>
#include <Renderer/OpenGL/RenderToolsGL.h>

namespace Ice {

static const std::vector<GLfloat> vQuad{ 
    -1.0f, 1.0f,
    -1.0f, -1.0f,
    1.0f, 1.0f,
    1.0f, -1.0f
};

RenderObjectGL BasePostProcessorGL::m_quad;

BasePostProcessorGL::BasePostProcessorGL(GLsizei nWidth, GLsizei nHeight)
    : m_fbo{ nWidth < 0 ? systemServices.getGraphicsSystem()->displayWidth() : nWidth, nHeight < 0 ? systemServices.getGraphicsSystem()->displayHeight() : nHeight }
{
    if (!bStaticInit) [[unlikely]] {
        m_quad = RenderToolsGL::loadVerticesToVAO(vQuad, 2);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);

        bStaticInit = true;
    }

    m_fbo.createTextureAttachment();
    m_fbo.unbind();

    m_shader.fromSource(getVertexShaderSource(), getFragmentShaderSource());
    m_shader.use();
    m_nTexUniformID = m_shader.getUniformLocation("tex");
    m_shader.loadInt(m_nTexUniformID, 0);
    m_shader.unuse();
}


GLuint BasePostProcessorGL::postProcess(GLuint nTextureID) {
    m_fbo.bind();
    glClear (GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    // Bind quad VAO
    glBindVertexArray(m_quad.vao());
    // Activate the shader
    m_shader.use();
    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, nTextureID);
    // Render the quad
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    // Cleanup
    glBindTexture(GL_TEXTURE_2D, 0);
    m_shader.unuse();
    glBindVertexArray(0);
    m_fbo.unbind();

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