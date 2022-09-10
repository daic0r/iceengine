#include <GL/glew.h>
#include <Renderer/OpenGL/VertexAttributeGL.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <Renderer/OpenGL/RenderToolsGL.h>

namespace Ice
{
    template<typename ElementType>
    VertexAttributeGL<ElementType>::~VertexAttributeGL() {
        glCall(glDeleteBuffers(1, &m_nVBO));
    }

    template<typename ElementType>
    void VertexAttributeGL<ElementType>::connect() noexcept {
        glCreateBuffers(1, &m_nVBO);
        glNamedBufferStorage(m_nVBO, base_t::m_vBuffer.size() * sizeof(ElementType), &base_t::m_vBuffer[0], m_usageFlags | GL_MAP_WRITE_BIT);

        std::size_t nNumElements{};
        std::size_t nElementSize{};
        GLenum type{};
        if constexpr(
            std::is_same_v<glm::vec2, ElementType> || std::is_same_v<glm::vec3, ElementType> || std::is_same_v<glm::vec4, ElementType> ||
            std::is_same_v<glm::ivec2, ElementType> || std::is_same_v<glm::ivec3, ElementType> || std::is_same_v<glm::ivec4, ElementType>
        )
        {
            nNumElements = ElementType::length();
            nElementSize = sizeof(typename ElementType::value_type);
            if constexpr (std::is_same_v<typename ElementType::value_type, int>)
                type = GL_INT;
            else
            if constexpr (std::is_same_v<typename ElementType::value_type, float>)
                type = GL_FLOAT;
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
        glVertexAttribPointer(base_t::index(), nNumElements, type, GL_FALSE, nElementSize * nNumElements, nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    template<typename ElementType>
    void VertexAttributeGL<ElementType>::enable() noexcept {
        glEnableVertexAttribArray(base_t::index());
    }

    template<typename ElementType>
    void VertexAttributeGL<ElementType>::disable() noexcept {
        glDisableVertexAttribArray(base_t::index());
    }

    template<typename ElementType>
    void DynamicVertexAttributeGL<ElementType>::connect() noexcept {
        VertexAttributeGL<ElementType>::m_usageFlags = GL_DYNAMIC_STORAGE_BIT;
        VertexAttributeGL<ElementType>::connect();
    }     

    template<typename ElementType>
    void DynamicVertexAttributeGL<ElementType>::beginUpdate() noexcept {
        glBindBuffer(GL_ARRAY_BUFFER, VertexAttributeGL<ElementType>::m_nVBO);
        m_pUpdateBuffer = static_cast<ElementType*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
    }
    template<typename ElementType>
    void DynamicVertexAttributeGL<ElementType>::doUpdate(std::size_t nIndex) noexcept {
        m_pUpdateBuffer[nIndex] = VertexAttribute<ElementType>::m_vBuffer.at(nIndex);
    }
    template<typename ElementType>
    void DynamicVertexAttributeGL<ElementType>::endUpdate() noexcept {
        glUnmapBuffer(GL_ARRAY_BUFFER);
        m_pUpdateBuffer = nullptr;
    }

    template class VertexAttributeGL<glm::vec2>;
    template class VertexAttributeGL<glm::vec3>;
    template class VertexAttributeGL<glm::vec4>;
    template class DynamicVertexAttributeGL<glm::vec2>;
    template class DynamicVertexAttributeGL<glm::vec3>;
    template class DynamicVertexAttributeGL<glm::vec4>;
} // namespace Ice
