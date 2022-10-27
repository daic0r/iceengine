#ifndef STATIC_TASK_H
#define STATIC_TASK_H

#include <cstddef>
#include <memory>
#include <array>

template<typename T, std::size_t MaxSize = 64ul>
class static_task;

template<typename R, typename... Args, std::size_t MaxSize>
class static_task<R(Args...), MaxSize> {
    class concept_interface_t;

    std::array<unsigned char, MaxSize + alignof(std::max_align_t)> m_buf;
    concept_interface_t* m_pFunc{};

public:
    constexpr static_task(std::nullptr_t) {}

    template<typename Func, typename = std::enable_if_t<!std::is_same_v<std::remove_reference_t<std::decay_t<Func>>, static_task>, void>>
    constexpr static_task(Func&& func) : m_pFunc{ reinterpret_cast<concept_interface_t*>(&m_buf[0]) }
    {
        static_assert(sizeof(Func) + alignof(Func) <= MaxSize + alignof(std::max_align_t));
        
        std::size_t nTempSize{ sizeof(m_buf) };
        if (!std::align(alignof(Func), sizeof(Func), reinterpret_cast<void*&>(m_pFunc), nTempSize))
            throw std::runtime_error("Not enough space to fit callable");
        new (reinterpret_cast<void*>(m_pFunc)) concrete_t<Func>{ std::forward<Func>(func) };
    }

    constexpr static_task(const static_task& rhs) : 
        m_pFunc{ rhs.m_pFunc->clone(reinterpret_cast<void*>(&m_buf[0])) }
    {}
    constexpr static_task& operator=(const static_task& rhs) {
        static_task copy{ rhs };
        copy.swap(*this);
        return *this;
    }
    constexpr ~static_task() { if (m_pFunc) m_pFunc->~concept_interface_t(); }

    template<typename... Ts>
    constexpr decltype(auto) operator()(Ts&&... args) const { return (*m_pFunc)(std::forward<Ts>(args)...); }

    constexpr operator bool() const noexcept { return m_pFunc != nullptr; }

private:
    class concept_interface_t {
    public:
        virtual constexpr R operator()(Args...) = 0;
        virtual constexpr concept_interface_t* clone(void*) const = 0;
        virtual constexpr ~concept_interface_t() = default;
    };

    template<typename Func>
    class concrete_t : public concept_interface_t {
    public:
        template<typename T = Func>
        constexpr concrete_t(T&& func) : m_func{ std::forward<T>(func) } {}
        constexpr R operator()(Args... args) override { return m_func(args...); }
        constexpr concrete_t* clone(void* pMem) const override { return new (pMem) concrete_t{ m_func }; }

    private:
        Func m_func;
    };

    constexpr void swap(static_task& rhs) noexcept {
        m_buf.swap(rhs.m_buf);
        using namespace std;
        swap(m_pFunc, rhs.m_pFunc);
    }

    friend constexpr void swap(static_task& lhs, static_task& rhs) noexcept {
        lhs.swap(rhs);
    }

};

#endif