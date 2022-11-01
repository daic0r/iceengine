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
    constexpr static_task() = default;
    constexpr static_task(std::nullptr_t) {}

    template<typename Func, typename = std::enable_if_t<!std::is_same_v<std::remove_reference_t<std::decay_t<Func>>, static_task>, void>>
    constexpr static_task(Func&& func) : m_pFunc{ reinterpret_cast<concept_interface_t*>(&m_buf[0]) }
    {
        static_assert(sizeof(concrete_t<Func>) + alignof(concrete_t<Func>) <= MaxSize + alignof(std::max_align_t));
        
        std::size_t nTempSize{ m_buf.size() };
        if (!std::align(alignof(concrete_t<Func>), sizeof(concrete_t<Func>), reinterpret_cast<void*&>(m_pFunc), nTempSize))
            throw std::runtime_error("Not enough space to fit callable");
        new (reinterpret_cast<void*>(m_pFunc)) concrete_t<Func>{ std::forward<Func>(func) };
    }

    constexpr static_task(const static_task& rhs) : 
        m_pFunc{ rhs.m_pFunc ? rhs.m_pFunc->clone(reinterpret_cast<void*>(&m_buf[0]), m_buf.size()) : nullptr }
    {}

    constexpr static_task& operator=(const static_task& rhs) {
        static_task copy{ rhs };
        copy.swap(*this);
        return *this;
    }
    constexpr static_task(static_task&&) noexcept = default;
    constexpr static_task& operator=(static_task&&) noexcept = default;

    constexpr ~static_task() { if (m_pFunc) m_pFunc->~concept_interface_t(); }

    template<typename... Ts>
    constexpr decltype(auto) operator()(Ts&&... args) const { return (*m_pFunc)(std::forward<Ts>(args)...); }

    constexpr operator bool() const noexcept { return m_pFunc != nullptr; }

private:
    class concept_interface_t {
    public:
        virtual constexpr R operator()(Args...) = 0;
        virtual constexpr concept_interface_t* clone(void* pMem, std::size_t nBufSize) const noexcept = 0;
        virtual constexpr ~concept_interface_t() = default;
    };

    template<typename Func>
    class concrete_t : public concept_interface_t {
    public:
        template<typename T = Func>
        constexpr concrete_t(T&& func) : m_func{ std::forward<T>(func) } {}
        constexpr R operator()(Args... args) override { return m_func(args...); }
        constexpr concrete_t* clone(void* pMem, std::size_t nBufSize) const noexcept override { 
            std::size_t nTempSize{ nBufSize };
            if (!std::align(alignof(concrete_t), sizeof(concrete_t), pMem, nTempSize)) 
                return nullptr;
            
            return new (pMem) concrete_t{ m_func };
        }

    private:
        Func m_func;
    };

    constexpr void swap(static_task& rhs) noexcept {
        std::array<unsigned char, std::tuple_size_v<decltype(m_buf)>> bufTemp;
        const auto pTemp = rhs.m_pFunc ? rhs.m_pFunc->clone(&bufTemp[0], m_buf.size()) : nullptr;
        rhs.m_pFunc = m_pFunc ? m_pFunc->clone(&rhs.m_buf[0], rhs.m_buf.size()) : nullptr;
        m_pFunc = pTemp ? pTemp->clone(&m_buf[0], m_buf.size()) : nullptr;
    }

    friend constexpr void swap(static_task& lhs, static_task& rhs) noexcept {
        lhs.swap(rhs);
    }

};

#endif