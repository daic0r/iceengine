#ifndef VertexAttributeBuffer_h
#define VertexAttributeBuffer_h

#include <vector>

namespace Ice
{
    class VertexAttribute {
    public:
        VertexAttribute(std::size_t nAttribIdx) : m_nAttributeIdx{ nAttribIdx } {}

        template<typename ElementType>
        void setData(std::vector<ElementType>* vBuf) {
            m_pvBuffer = vBuf;
        }
        constexpr auto index() const noexcept { return m_nAttributeIdx; }
        constexpr bool operator<(const VertexAttribute& other) const noexcept { return m_nAttributeIdx < other.m_nAttributeIdx; }

        template<typename T>
        constexpr const T& at(std::size_t nIndex) const { 
            auto& buf = *static_cast<std::vector<T>*>(m_pvBuffer);
            return buf.at(nIndex);
        }
 
        virtual void connect() noexcept = 0;
        virtual void enable() noexcept = 0;
        virtual void disable() noexcept = 0;
        virtual ~VertexAttribute() = default;

    protected:
        std::size_t m_nAttributeIdx{};
        void* m_pvBuffer{};
    };

    class DynamicVertexAttribute : virtual public VertexAttribute {
        virtual void beginUpdate() noexcept = 0;
        virtual void doUpdate(std::size_t nIndex) noexcept = 0;
        virtual void endUpdate() noexcept = 0;

    public:
        using VertexAttribute::VertexAttribute;

        template<typename T>
        constexpr void update(std::size_t nIndex, T&& value) { 
            auto& buf = *static_cast<std::vector<std::remove_cvref_t<T>>*>(m_pvBuffer);
            buf.at(nIndex) = std::forward<T>(value);
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