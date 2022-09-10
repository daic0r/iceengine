#ifndef VAO_h
#define VAO_h

#ifdef RENDERER_OPEN_GL

#include <SDL2/SDL_opengl.h>
#include <Renderer/OpenGL/VertexAttributeGL.h>
#include <memory>
#include <map>
#include <Renderer/OpenGL/VBO.h>
#include <optional>
#include <vector>

namespace Ice
{
    class VAO {
        VBO _makeVBO(const std::vector<unsigned int>&) noexcept;

    public:
        VAO() = default;
        VAO(GLuint nVAO) noexcept;
        VAO(const VAO&) = delete;
        VAO& operator=(const VAO&) = delete;
        VAO(VAO&&) noexcept;
        VAO& operator=(VAO&&) noexcept;
        ~VAO();

        bool operator==(const VAO& other) const noexcept { return m_nVAO == other.m_nVAO; }
        bool operator!=(const VAO& other) const noexcept { return !(*this == other); }

        // Create the actual VAO
        void create() noexcept;
        void bind() const noexcept;
        void unbind() const noexcept;
        void cleanup() noexcept;
        VertexAttribute& attribute(GLuint nIndex);
        template<typename ElementType>
        VertexAttribute& addStaticVertexAttribute(GLuint nIndex) {
            auto [iter, _] = m_mAttributes.emplace(nIndex, std::make_unique<VertexAttributeGL<ElementType>>(nIndex));
            return *iter->second.get();
        }
        template<typename ElementType>
        VertexAttribute& addDynamicVertexAttribute(GLuint nIndex) {
            auto [iter, _] = m_mAttributes.emplace(nIndex, std::make_unique<DynamicVertexAttributeGL<ElementType>>(nIndex));
            return *iter->second.get();
        }
        void setIndexBuffer(const std::vector<unsigned int>& vBuf);
        void setIndexBuffer(std::vector<unsigned int>&&);
        decltype(auto) indexBuffer() const noexcept { return (m_vIndexBuffer.value()); }
        void bindIndexVBO() const noexcept;
        const VBO& indexVBO() const noexcept { return m_indexVBO.value(); }
        VBO& indexVBO() noexcept { return m_indexVBO.value(); }

        constexpr auto id() const noexcept { return m_nVAO; }
        constexpr operator GLuint() const noexcept { return id(); }

        void swap(VAO& other) noexcept;
        friend void swap(VAO& lhs, VAO& rhs) noexcept {
            lhs.swap(rhs);
        }

    private:
        GLuint m_nVAO{};
        std::map<GLuint, std::unique_ptr<VertexAttribute>> m_mAttributes{};
        std::optional<VBO> m_indexVBO;
        std::optional<std::vector<unsigned int>> m_vIndexBuffer;
 
    };
} // namespace Ice


#endif

#endif