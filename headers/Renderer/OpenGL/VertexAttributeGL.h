#ifndef VertexAttributeGL_h
#define VertexAttributeGL_h

#ifdef RENDERER_OPEN_GL

#include <SDL2/SDL_opengl.h>
#include <Renderer/VertexAttribute.h>
#include <type_traits>

namespace Ice
{
    template<typename ElementType>
    class VertexAttributeGL : virtual public VertexAttribute<ElementType> {
        using base_t = VertexAttribute<ElementType>; 
    protected:
        GLenum m_usageFlags = 0;
        GLuint m_nVBO{};

    public:
        using VertexAttribute<ElementType>::VertexAttribute;
        virtual ~VertexAttributeGL();

        void connect() noexcept override;
        void enable() noexcept override;
        void disable() noexcept override;
    };

    template<typename ElementType>
    class DynamicVertexAttributeGL : public VertexAttributeGL<ElementType>, public DynamicVertexAttribute<ElementType> {
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