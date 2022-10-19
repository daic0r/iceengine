#include <glad/glad.h>
#include <Renderer/OpenGL/VAO.h>
#include <utility>

namespace Ice
{
    VAO::VAO(GLuint nVAO) noexcept : m_nVAO{ nVAO } {}

    VAO::VAO(VAO&& other) noexcept {
        m_nVAO = std::exchange(other.m_nVAO, 0);
        m_mAttributes = std::move(other.m_mAttributes);
        other.m_mAttributes.clear();
        m_indexVBO = std::exchange(other.m_indexVBO, std::nullopt);
        m_vIndexBuffer = std::exchange(other.m_vIndexBuffer, std::nullopt);
    }
    VAO& VAO::operator=(VAO&& other) noexcept {
        VAO copy{ std::move(other) };
        copy.swap(*this);
        return *this;
    }
    VAO::~VAO() {
        cleanup();
    }

    void VAO::create() noexcept {
        if (m_nVAO == 0)
            glGenVertexArrays(1, &m_nVAO);
    }

    void VAO::bind() const noexcept {
        glBindVertexArray(m_nVAO);
    }

    void VAO::unbind() const noexcept {
        glBindVertexArray(0);
    }

    void VAO::cleanup() noexcept {
        if (m_nVAO != 0) {
            glDeleteVertexArrays(1, &m_nVAO);
            m_nVAO = 0;
        }
    }

    VertexAttribute& VAO::attribute(GLuint nIndex) {
        return *m_mAttributes.at(nIndex).get();
    }

    void VAO::setIndexBuffer(const std::vector<unsigned int>& vBuf) {
        m_indexVBO.reset();
        m_vIndexBuffer.reset();
        m_indexVBO = std::move(_makeVBO(vBuf));
        m_vIndexBuffer = vBuf;
    }

    void VAO::setIndexBuffer(std::vector<unsigned int>&& vBuf) {
        m_indexVBO.reset();
        m_vIndexBuffer.reset();
        m_indexVBO = std::move(_makeVBO(vBuf));
        m_vIndexBuffer = std::move(vBuf);
    }

    VBO VAO::_makeVBO(const std::vector<unsigned int>& vBuf) noexcept {
        VBO vbo;
        vbo.create();
        glNamedBufferStorage(vbo, vBuf.size() * sizeof(unsigned int), &vBuf[0], 0);
        return vbo;
    }

    void VAO::bindIndexVBO() const noexcept {
        indexVBO().bind(GL_ELEMENT_ARRAY_BUFFER);
    }

    void VAO::swap(VAO& other) noexcept {
        using std::swap;
        swap(m_nVAO, other.m_nVAO);
        swap(m_mAttributes, other.m_mAttributes); 
        swap(m_indexVBO, other.m_indexVBO);
        swap(m_vIndexBuffer, other.m_vIndexBuffer);
    }
} // namespace Ice
