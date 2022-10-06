#include <GL/glad.h>
#include <Renderer/OpenGL/VBO.h>
#include <utility>

namespace Ice
{
    VBO::VBO(GLuint nVBO) noexcept : m_nVBO{ nVBO } {}

    VBO::VBO(VBO&& other) noexcept {
        m_nVBO = std::exchange(other.m_nVBO, 0);
    }
    VBO& VBO::operator=(VBO&& other) noexcept {
        VBO copy{ std::move(other) };
        copy.swap(*this);
        return *this;
    }
    VBO::~VBO() {
        cleanup();
    }

    void VBO::create() noexcept {
        if (m_nVBO == 0)
            glCreateBuffers(1, &m_nVBO);
    }

    void VBO::bind(GLenum nTarget) const noexcept {
        glBindBuffer(nTarget, m_nVBO);
    }

    void VBO::cleanup() noexcept {
        if (m_nVBO != 0) {
            glDeleteBuffers(1, &m_nVBO);
            m_nVBO = 0;
        }
    }

    void VBO::swap(VBO& other) noexcept {
        using std::swap;
        swap(m_nVBO, other.m_nVBO);
    }
} // namespace Ice
