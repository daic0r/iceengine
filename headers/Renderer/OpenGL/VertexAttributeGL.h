#ifndef VertexAttributeGL_h
#define VertexAttributeGL_h

#ifdef RENDERER_OPEN_GL

#include <SDL2/SDL_opengl.h>
#include <Renderer/VertexAttribute.h>
#include <type_traits>
#include <Renderer/OpenGL/VBO.h>

namespace Ice
{
    template<typename ElementType>
    class VertexAttributeGL : virtual public VertexAttribute {
    protected:
        GLenum m_usageFlags = 0;
        VBO m_VBO{};

    public:
        using VertexAttribute::VertexAttribute;

        auto operator<=>(const VertexAttributeGL&) const = default;

        void connect() noexcept override;
        void enable() noexcept override;
        void disable() noexcept override;
        
        const std::vector<ElementType>& buffer() const noexcept { return std::any_cast<const std::vector<ElementType>&>(m_vBuffer); }
    };

    template<typename ElementType>
    class DynamicVertexAttributeGL : public VertexAttributeGL<ElementType>, public DynamicVertexAttribute {
        
    public:
        using VertexAttributeGL<ElementType>::VertexAttributeGL;

        void connect() noexcept override;

    private:
        ElementType* m_pUpdateBuffer{};

        void beginUpdate() noexcept override; 
        void doUpdate(std::size_t nIndex) noexcept override;
        void endUpdate() noexcept override;
    };
} // namespace Ice

#endif

#endif