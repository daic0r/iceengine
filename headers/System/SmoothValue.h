#ifndef SmoothValue_h
#define SmoothValue_h

#include <type_traits>

namespace Ice {

    template<typename T>
    class SmoothValue {
        static_assert(std::is_default_constructible_v<T>, "Type for SmoothValue must be default-constructible");

        T m_target{};
        T m_value{};
        float m_fAgility{ 10.0f };

    public:
        SmoothValue() = default;
        SmoothValue(T val, float fAgility = 10.0f) : m_target{ val }, m_value{ val }, m_fAgility{ fAgility } {}
        SmoothValue(const SmoothValue&) = default;
        SmoothValue& operator=(const SmoothValue&) = default;
        SmoothValue& operator=(T val) { m_target = val; return *this; }   // Set target, is this correct?

        operator const T& () const noexcept { return m_value; }
        const T& value() const noexcept { return m_value; }
        void setValue(T val) { m_value = val; }
        
        const T& target() const noexcept { return m_target; }
        void setTarget(T val) { m_target = val; }
        void incTarget(T val) { m_target += val; }

        void force(T val) { setValue(val); setTarget(val); }
        void force() { setValue(target()); }

        auto agility() const noexcept { return m_fAgility; }
        void setAgility(float f) noexcept { m_fAgility = f; }

        void update(float fDeltaTime) {
            const auto diff = target() - value();
            m_value += diff * fDeltaTime * agility();
        }

    };

}

#endif