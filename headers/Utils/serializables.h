#ifndef serializables_h
#define serializables_h

#include <Utils/TemplateHelpers.h>
#include <nlohmann/json.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

/*
template<typename... Args>
struct serializables {
private:
	std::array<void*, sizeof...(Args)> m_arAttribs;
	static inline constexpr auto SIZE = sizeof...(Args);

public:
	template<typename... Ts> requires(sizeof...(Ts) == sizeof...(Args))
	serializables(Ts&... attribs) : m_arAttribs{ static_cast<void*>(std::addressof(attribs))... } {}

	template<std::size_t N>
	constexpr const auto& get() const {
		return *static_cast<detail::nth_type_of<N, Args...>*>(m_arAttribs[N]);
	}

	constexpr auto size() const noexcept { return SIZE; }

};

template<typename... Args>
serializables(Args&...) -> serializables<Args...>;
*/

namespace Ice {
template<typename... Args>
struct serializables {
private:
	std::array<std::pair<std::string_view, void*>, sizeof...(Args)> m_arAttribs;
	static inline constexpr auto SIZE = sizeof...(Args);

public:
	template<typename... Pairs> requires(sizeof...(Pairs) == sizeof...(Args))
	constexpr serializables(Pairs&&... attribs) : m_arAttribs{ std::make_pair(attribs.first, static_cast<void*>(std::addressof(attribs.second.ref())))... } {}

	template<std::size_t N>
	constexpr auto getName() const { return m_arAttribs[N].first; }

	template<std::size_t N>
	constexpr auto& get() const {
		return *static_cast<nth_type_of<N, Args...>*>(m_arAttribs[N].second);
	}

	constexpr auto size() const noexcept { return SIZE; }

	constexpr auto serialize(nlohmann::json& j) const {
		return serialize(j, std::index_sequence_for<Args...>{});
	}

	constexpr void deserialize(const nlohmann::json& j) {
		deserialize(j, std::index_sequence_for<Args...>{});
	}

private:
	template<std::size_t... Ns>
    constexpr auto serialize(nlohmann::json& j, std::index_sequence<Ns...>) const {
		(j.emplace(getName<Ns>(), get<Ns>()), ...); 
		return j;
    }

	template<std::size_t... Ns>
	constexpr void deserialize(const nlohmann::json& j, std::index_sequence<Ns...>) {
		((get<Ns>() = j.at(getName<Ns>().data()).template get<nth_type_of<Ns, Args...>>()), ...);
	}
};

template<template<typename,typename> typename... Pairs, template<typename> typename RefPasser, typename... Names, typename... Attribs>
serializables(Pairs<Names,RefPasser<Attribs>>&&...) -> serializables<Attribs...>;

template<typename T>
struct ref_passer {
private:
	T& m_ref;
public:
	ref_passer(T& ref) : m_ref{ ref } {}

	operator T&() { return m_ref; }
	T& ref() { return m_ref; }
};

template<typename T>
ref_passer(T&) -> ref_passer<T>;

#define BEGIN_DECLARE_SERIALIZABLE_ATTRIBS() auto getSerializables() {\
    return Ice::serializables{

#define SERIALIZABLE_ATTRIB(name, var) std::make_pair(#name, Ice::ref_passer{ var }),

#define END_DECLARE_SERIALIZABLE_ATTRIBS() };\
}

}

namespace glm {
    inline void to_json(nlohmann::json& j, const vec<3, float, defaultp>& v) {
        j["x"] = v.x;
        j["y"] = v.y;
        j["z"] = v.z;
    }

    inline void from_json(const nlohmann::json& j, vec<3, float, defaultp>& v) {
        v.x = j.at("x").template get<float>();
        v.y = j.at("y").template get<float>();
        v.z = j.at("z").template get<float>();
    }

    inline void to_json(nlohmann::json& j, const vec<4, float, defaultp>& v) {
        j["x"] = v.x;
        j["y"] = v.y;
        j["z"] = v.z;
        j["w"] = v.w;
    }

    inline void from_json(const nlohmann::json& j, vec<4, float, defaultp>& v) {
        v.x = j.at("x").template get<float>();
        v.y = j.at("y").template get<float>();
        v.z = j.at("z").template get<float>();
        v.w = j.at("w").template get<float>();
    }

	inline void to_json(nlohmann::json& j, const glm::mat4& m) {
		j["col1"] = m[0];
		j["col2"] = m[1];
		j["col3"] = m[2];
		j["col4"] = m[3];
	}

	inline void from_json(const nlohmann::json& j, glm::mat4& m) {
		m[0] = j.at("col1").template get<glm::vec4>();
		m[1] = j.at("col2").template get<glm::vec4>();
		m[2] = j.at("col3").template get<glm::vec4>();
		m[3] = j.at("col4").template get<glm::vec4>();
	}
}
#endif

