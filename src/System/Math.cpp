#include <System/Math.h>

namespace Ice::Math
{
    namespace detail {
        std::random_device rd{};
        std::mt19937 mersenne_twister{rd()};
    }

    std::array<float, 3> getBarycentricCoords(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v) noexcept {
        glm::mat2 invT{ v1.x - v3.x, v1.y - v3.y, v2.x - v3.x, v2.y - v3.y };
        invT = glm::inverse(invT);
        
        const auto lambdas = invT * (v - v3);
        return { lambdas[0], lambdas[1], 1 - lambdas[0] - lambdas[1] };
    }

    int random(int nMin, int nMax) {
        std::uniform_int_distribution<> dist{ nMin, nMax };
        return dist(detail::mersenne_twister);
    }

} // namespace Ice
