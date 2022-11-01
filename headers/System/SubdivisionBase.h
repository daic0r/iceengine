#ifndef SubdivisionBase_h
#define SubdivisionBase_h

#include <System/Types.h>
#include <System/static_task.h>
#include <functional>

namespace Ice
{
    class Frustum;
    class Ray;

    template<typename LeafNodeContainerType, typename ValueType>
    class SubdivisionBase {
    public:
        using collection_func_t = std::function<void(const LeafNodeContainerType&)>;
        using onhitleafnodefunc_t = static_task<SubdivisionIntersectionBehavior(const LeafNodeContainerType&)>;
        using emplace_func_t = std::function<void(LeafNodeContainerType&, const ValueType&)>;

    protected:
        collection_func_t m_getVisibleObjectCollectionFunc;
        emplace_func_t m_emplaceFunc;

    public:
        void setGetVisibleObjectCollectionFunc(collection_func_t f) { m_getVisibleObjectCollectionFunc = std::move(f); }
        void setEmplaceFunc(emplace_func_t f) { m_emplaceFunc = std::move(f); }

        const auto& getVisibleObjectCollectionFunc() const noexcept { return m_getVisibleObjectCollectionFunc; }
        const auto& emplaceFunc() const noexcept { return m_emplaceFunc; }
    };
} // namespace Ice



#endif