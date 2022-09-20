#include <System/Triangle.h>
#include <System/Ray.h>
#include <glm/geometric.hpp>
#include <System/Math.h>

namespace Ice
{
    Triangle::IntersectResult Triangle::intersects(const Ray& ray, Plane projectionPlane) const noexcept {
        IntersectResult ret;
        ret.bIntersects = false;

        const auto v1 = glm::normalize(m_vertices[1] - m_vertices[0]);
        const auto v2 = glm::normalize(m_vertices[2] - m_vertices[0]);
        const auto normal = glm::cross(v1, v2);

        const auto fDenominator = glm::dot(normal, ray.direction());
        if (Math::equal(fDenominator, 0.0f))
            return ret;
        const auto D = glm::dot(normal, m_vertices[0]); 
        const auto t = -(glm::dot(normal, ray.origin()) - D) / fDenominator;
        if (t < 0.0f)
            return ret;

        ret.point = ray.origin() + t * ray.direction();

        std::array<int, 2> arComponents;
        switch (projectionPlane) {
            case Plane::YZ:
                arComponents[0] = 1;
                arComponents[1] = 2;
                break;
            case Plane::XZ:
                arComponents[0] = 0;
                arComponents[1] = 2;
                break;
            case Plane::XY:
                arComponents[0] = 0;
                arComponents[1] = 1;
                break;
        }
        const auto lambdas = Math::getBarycentricCoords(glm::vec2{ m_vertices[0][arComponents[0]], m_vertices[0][arComponents[1]] }, glm::vec2{ m_vertices[1][arComponents[0]], m_vertices[1][arComponents[1]] }, glm::vec2{ m_vertices[2][arComponents[0]], m_vertices[2][arComponents[1]] }, glm::vec2{ ret.point[arComponents[0]], ret.point[arComponents[1]] });
        ret.barycentric = { std::get<0>(lambdas), std::get<1>(lambdas), std::get<2>(lambdas) };
        bool bFound{true};
        for (std::size_t i{}; i < 3; ++i) {
            if (ret.barycentric[i] < 0.0f || ret.barycentric[i] > 1.0f) {
                bFound = false;
                break;
            }
        }
        if (bFound) {
            ret.bIntersects = true;
        }
        return ret;
    }
} // namespace Ice
