#pragma once

#include <queue>

namespace Ice {

template<
    class T,
    class Container = std::vector<T>,
    class Compare = std::less<typename Container::value_type>
> class searchable_priority_queue : public std::priority_queue<T, Container, Compare>
{
public:
    typedef typename
        std::priority_queue<
        T,
        Container,
        Compare>::container_type::const_iterator const_iterator;

    const_iterator find(const T& val) const noexcept
    {
        auto first = this->c.cbegin();
        auto last = this->c.cend();
        while (first != last) {
            if (*first == val) return first;
            ++first;
        }
        return last;
    }

    const_iterator find(const std::function<bool(const T&)>& pred) const noexcept
    {
        auto first = this->c.cbegin();
        auto last = this->c.cend();
        while (first != last) {
            if (pred(*first)) return first;
            ++first;
        }
        return last;
    }

    const_iterator cend() const noexcept {
        return this->c.cend();
    }
};

}