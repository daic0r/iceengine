//
//  vector_front_insert_iterator.h
//  IceCraft
//
//  Created by Matthias Gruen on 01.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef vector_front_insert_iterator_h
#define vector_front_insert_iterator_h

#include <iterator>

namespace Ice {
 
template <class _Container>
class vector_front_insert_iterator { // wrap pushes to front of vector as output iterator
public:
    using iterator_category = std::output_iterator_tag;
    using value_type = void;
    using pointer = void;
    using reference = void;

    using container_type = _Container;

#ifdef __cpp_lib_concepts
    using difference_type = ptrdiff_t;

    constexpr back_insert_iterator() noexcept = default;
#else // ^^^ __cpp_lib_concepts / !__cpp_lib_concepts vvv
    using difference_type = void;
#endif // __cpp_lib_concepts

    inline explicit vector_front_insert_iterator(_Container& _Cont) noexcept /* strengthened */
                    : container(std::addressof(_Cont)) {}

    inline vector_front_insert_iterator& operator=(const typename _Container::value_type& _Val) {
                    container->insert(container->begin(), _Val);
                    return *this;
    }

    inline vector_front_insert_iterator& operator=(typename _Container::value_type&& _Val) {
                    container->insert(container->begin(), std::move(_Val));
                    return *this;
    }

    inline vector_front_insert_iterator& operator*() noexcept /* strengthened */ {
                    return *this;
    }

    inline vector_front_insert_iterator& operator++() noexcept /* strengthened */ {
                    return *this;
    }

    inline vector_front_insert_iterator operator++(int) noexcept /* strengthened */ {
                    return *this;
    }

protected:
    _Container* container = nullptr;
};
 
template <class _Container>
inline vector_front_insert_iterator<_Container> vector_front_inserter(_Container& _Cont) noexcept /* strengthened */ {
    // return a back_insert_iterator
    return vector_front_insert_iterator<_Container>(_Cont);
}

}
 
#endif /* vector_front_insert_iterator_h */
