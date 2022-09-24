#ifndef Triangle_h
#define Triangle_h

#include <glm/vec3.hpp>
#include <array>
#include <System/Types.h>

namespace Ice
{
    class Ray;

    class Triangle {
        std::array<glm::vec3, 3> m_vertices;

    public:
        struct IntersectResult {
            bool bIntersects{};
            glm::vec3 point;
            std::array<float, 3> barycentric;
        };

        Triangle() = default;
        constexpr Triangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) : m_vertices{v0,v1,v2} {}
        constexpr Triangle(const std::array<glm::vec3, 3>& ar) : Triangle{ ar[0], ar[1], ar[2] } {}

        constexpr const glm::vec3& operator[](std::size_t n) const noexcept { return m_vertices[n]; }
        constexpr glm::vec3& operator[](std::size_t n) noexcept { return m_vertices[n]; }

        constexpr const auto& toArray() const noexcept { return m_vertices; }
        constexpr operator std::array<glm::vec3, 3> const&() const noexcept { return toArray(); }

        IntersectResult intersects(const Ray&, Plane projectionPlane = Plane::XZ) const noexcept;
    };
    
} // namespace Ice



#endif