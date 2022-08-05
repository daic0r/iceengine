//
//  TemplateHelpers.h
//  IceCraft
//
//  Created by Matthias Gruen on 05.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef TemplateHelpers_h
#define TemplateHelpers_h

#include <System/Percent.h>
#include <System/Pixels.h>
#include <utility>

//////////////////////////////////////////////////////
// ostream
// VERBOSE IMPLEMENTATION
/*
template<typename T>
class has_ostream_operator {

    using NoType = std::uint8_t;
    using YesType = std::uint16_t;
    
    template<typename C> static YesType test(decltype(std::declval<std::ostream&>() << std::declval<C>()));
    template<typename C> static NoType test(...);

public:
    
    enum { value = sizeof(test<T>(std::declval<std::ostream&>())) == sizeof(YesType) };
};

template<typename T> inline constexpr bool has_ostream_operator_v = has_ostream_operator<T>::value;
 */

namespace Ice {

template<typename T, typename U = void>
struct has_ostream_operator : std::false_type {};

template<typename T>
struct has_ostream_operator<T, typename std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>> : std::true_type {};

template<typename T> static inline constexpr bool has_ostream_operator_v = has_ostream_operator<T>::value;
//////////////////////////////////////////////////////


//////////////////////////////////////////////////////
// istream
template<typename T, typename U = void>
struct has_istream_operator : std::false_type {};

template<typename T>
struct has_istream_operator<T, typename std::void_t<decltype(std::declval<std::istream&>() >> std::declval<T&>())>> : std::true_type {};

template<typename T> static inline constexpr bool has_istream_operator_v = has_istream_operator<T>::value;
//////////////////////////////////////////////////////

template<typename T>
concept IsMeasurementUnit = std::is_same_v<std::remove_cvref_t<T>, Pixels> || std::is_same_v<std::remove_cvref_t<T>, Percent> || std::is_same_v<std::remove_cvref_t<T>, NormalizedPixels>;

template<typename T>
concept IsSavable = requires (T x) {
	x.getSerializables();
};

template<typename T>
concept IsSavableOld = has_ostream_operator_v<T>;

}

#define DECLARE_SERIALIZABLES(...) Ice::serializables m_serializables{ __VA_ARGS__ };

namespace detail {
	template<size_t N, typename T, typename... Args>
	constexpr auto getPackElement(T&& val, Args&&... args) noexcept {
		if constexpr (N == 0)
			return val;
		else
			return getPackElement<N - 1>(args...);
	}

}

template<size_t N, typename... Args>
constexpr auto getPackElement(Args&&... args) noexcept {
	static_assert(N < sizeof...(Args));
	return detail::getPackElement<N>(args...);
}
 
template<int N, typename... Ts> using nth_type_of =
    typename std::tuple_element<N, std::tuple<Ts...>>::type;


template<typename... Ts>
struct visitor : Ts... {
	using Ts::operator()...;
};
template<typename... Ts>
visitor(Ts...) -> visitor<Ts...>;

#endif /* TemplateHelpers_h */
