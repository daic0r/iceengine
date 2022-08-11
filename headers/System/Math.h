#ifndef Math_h
#define Math_h

#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/mat2x2.hpp>
#include <tuple>
#include <random>

namespace Ice::Math {

    template<typename T>
    static inline constexpr T Zero = T{ 0 };
    template<typename T>
    static inline constexpr T One = T{ 1 };

    template<typename T>
    static inline constexpr T Epsilon = T{ .00001 };
    
    template<typename T>
    T abs(T x) noexcept {
        if (x < Zero<T>)
            return static_cast<T>(-1) * x;
        return x;
    }

    template<typename T>
    T sgn(T x) {
        return x < Zero<T> ? static_cast<T>(-1) : (x > Zero<T> ? One<T> : Zero<T>);
    }

    template<typename T>
    constexpr T clamp(T x, T x0, T x1) {
        return x < x0 ? x0 : (x > x1 ? x1 : x); 
    }

    template<typename T>
    constexpr T smoothstep(T x) {
        if (x <= Zero<T>)
            return Zero<T>;
        if (x >= One<T>)
            return One<T>;
        const auto squared = x * x;
        return squared * (T{ 3 } - T{ 2 } * x);
    }

    template<typename T>
    constexpr T smootherstep(T x) {
        if (x <= Zero<T>)
            return Zero<T>;
        if (x >= One<T>)
            return One<T>;
        const auto cubic = x * x * x;
        return cubic * (T{ 6 } * x * x - T{ 15 } * x + T{ 10 });
    }

    template<typename T>
    constexpr T map0to1Range(T x, T lowerBound, T upperBound) {
        return (x - lowerBound) / (upperBound - lowerBound);
    }

    template<typename T>
    constexpr T mix(T left, T right, float fBlendFactor) {
        fBlendFactor = clamp(fBlendFactor, 0.0f, 1.0f);
        return left * (1.0f - fBlendFactor) + right * fBlendFactor;
    }

    template<typename T>
    constexpr T degToRad(T deg) {
        return deg * (M_PI / T{ 180 });
    }

    template<typename T>
    constexpr auto equal(T a, T b, T epsilon = Epsilon<T>) {
        return abs(a - b) < epsilon;
    } 

    namespace detail
    {
        template<typename T>
        constexpr T sqrtNewtonRaphson(T x, T curr, T prev)
        {
            return abs(curr - prev) < static_cast<T>(.0001)
                ? curr
                : sqrtNewtonRaphson(x, static_cast<T>(0.5) * (curr + x / curr), curr);
        }
    }

    /*
    * Constexpr version of the square root
    * Return value:
    *   - For a finite and non-negative value of "x", returns an approximation for the square root of "x"
    *   - Otherwise, returns NaN
    */
    template<typename T>
    constexpr T sqrt(T x)
    {
        return x >= Zero<T> && x < std::numeric_limits<T>::infinity()
            ? detail::sqrtNewtonRaphson(x, x, Zero<T>)
            : std::numeric_limits<T>::quiet_NaN();
    }

    int random(int nMin, int nMax);    
    std::tuple<float, float, float> getBarycentricCoords(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v) noexcept;
}

#endif