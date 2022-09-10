#ifndef VertexAttributeBuffer_h
#define VertexAttributeBuffer_h

#include <vector>

namespace Ice
{
    template<typename ElementType>
    class VertexAttribute {
    public:
        constexpr VertexAttribute(std::size_t nAttribIdx, const std::vector<ElementType>& buf) : m_nAttributeIdx{ nAttribIdx }, m_vBuffer{ buf } {}
        constexpr VertexAttribute(std::size_t nAttribIdx, std::vector<ElementType>&& buf) : m_nAttributeIdx{ nAttribIdx }, m_vBuffer{ std::move(buf) } {}

        constexpr auto index() const noexcept { return m_nAttributeIdx; }
        constexpr decltype(auto) buffer() const noexcept { return (m_vBuffer); }
        constexpr const ElementType& at(std::size_t nIndex) const { return m_vBuffer.at(nIndex); }

        virtual void connect() noexcept = 0;
        virtual void enable() noexcept = 0;
        virtual void disable() noexcept = 0;
        virtual ~VertexAttribute() = default;

    protected:
        std::size_t m_nAttributeIdx{};
        std::vector<ElementType> m_vBuffer;
    };

    template<typename ElementType>
    class DynamicVertexAttribute : virtual public VertexAttribute<ElementType> {
        virtual void beginUpdate() noexcept = 0;
        virtual void doUpdate(std::size_t nIndex) noexcept = 0;
        virtual void endUpdate() noexcept = 0;

    public:
        using VertexAttribute<ElementType>::VertexAttribute;

        template<typename T = ElementType>
        constexpr void update(std::size_t nIndex, T&& value) { 
            VertexAttribute<ElementType>::m_vBuffer.at(nIndex) = std::forward<T>(value);
            m_vUpdates.emplace_back(nIndex);
        }

        void commitUpdates() noexcept {
            beginUpdate();
            for (const auto& nIndex : m_vUpdates) {
                doUpdate(nIndex);
            }
            endUpdate();
            m_vUpdates.clear();
        }

    private:
        std::vector<std::size_t> m_vUpdates;
    };
} // namespace Ice


#endif