#ifndef VBO_h
#define VBO_h

#ifdef RENDERER_OPEN_GL

#include <SDL2/SDL_opengl.h>

namespace Ice
{
    class VBO {
        GLuint m_nVBO{};

    public:
        VBO() = default;
        VBO(GLuint nVBO) noexcept;
        VBO(const VBO&) = delete;
        VBO& operator=(const VBO&) = delete;
        VBO(VBO&&) noexcept;
        VBO& operator=(VBO&&) noexcept;
        ~VBO();

        auto operator<=>(const VBO& other) const = default;

        void create() noexcept;
        void bind(GLenum nTarget) const noexcept;
        void cleanup() noexcept;

        constexpr auto id() const noexcept { return m_nVBO; }
        constexpr operator GLuint() const noexcept { return id(); }

        void swap(VBO& other) noexcept;
        friend void swap(VBO& lhs, VBO& rhs) noexcept {
            lhs.swap(rhs);
        }
    };
} // namespace Ice


#endif


#endif