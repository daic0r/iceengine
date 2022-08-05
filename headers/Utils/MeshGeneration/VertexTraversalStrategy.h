#ifndef VertexTraversalStrategy_h
#define VertexTraversalStrategy_h

#include <tuple>

namespace Ice::MeshGeneration
{
    struct VertexTraversalStrategy {
        struct Direction {
            int x, z;
        };

        std::array<Direction, 6> m_arSteps;
        std::size_t m_nNumSteps{};
    public:
        static constexpr auto TOP_LEFT = Direction{ 0, 0 };
        static constexpr auto TOP_RIGHT  = Direction{ 1, 0 };
        static constexpr auto BOTTOM_LEFT = Direction{ 0, 1 };
        static constexpr auto BOTTOM_RIGHT = Direction{ 1, 1 };

        template<typename... Directions>
        constexpr VertexTraversalStrategy(Directions... dirs)
            : m_arSteps{ dirs... }, m_nNumSteps{ sizeof...(dirs) }
        {}

        constexpr auto begin() const noexcept { return m_arSteps.begin(); }
        constexpr auto end() const noexcept { return m_arSteps.begin() + m_nNumSteps; }
        constexpr auto size() const noexcept { return m_nNumSteps; }
        constexpr auto operator[](std::size_t idx) const noexcept { return m_arSteps[idx]; }
    };

    static constexpr auto LowPolyTerrainQuadVariant1UpperPortionStrategy = VertexTraversalStrategy { 
        VertexTraversalStrategy::TOP_LEFT,
        VertexTraversalStrategy::BOTTOM_LEFT,
        VertexTraversalStrategy::TOP_RIGHT,
        VertexTraversalStrategy::TOP_RIGHT,
        VertexTraversalStrategy::BOTTOM_LEFT,
        VertexTraversalStrategy::BOTTOM_RIGHT
    };
    static constexpr auto LowPolyTerrainQuadVariant2UpperPortionStrategy = VertexTraversalStrategy { 
        VertexTraversalStrategy::TOP_LEFT,
        VertexTraversalStrategy::BOTTOM_LEFT,
        VertexTraversalStrategy::BOTTOM_RIGHT,
        VertexTraversalStrategy::TOP_RIGHT,
        VertexTraversalStrategy::TOP_LEFT,
        VertexTraversalStrategy::BOTTOM_RIGHT
    };
    static constexpr auto LowPolyTerrainQuadVariant1LastRowStrategy = VertexTraversalStrategy { 
        VertexTraversalStrategy::TOP_LEFT,
        VertexTraversalStrategy::BOTTOM_LEFT,
        VertexTraversalStrategy::TOP_RIGHT,
        VertexTraversalStrategy::BOTTOM_RIGHT,
        VertexTraversalStrategy::TOP_RIGHT,
        VertexTraversalStrategy::BOTTOM_LEFT
    };
    static constexpr auto LowPolyTerrainQuadVariant2LastRowStrategy = VertexTraversalStrategy { 
        VertexTraversalStrategy::TOP_LEFT,
        VertexTraversalStrategy::BOTTOM_LEFT,
        VertexTraversalStrategy::BOTTOM_RIGHT,
        VertexTraversalStrategy::BOTTOM_RIGHT,
        VertexTraversalStrategy::TOP_RIGHT,
        VertexTraversalStrategy::TOP_LEFT
    };
} // namespace Ice


#endif