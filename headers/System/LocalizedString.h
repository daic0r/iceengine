#ifndef LocalizedString_h
#define LocalizedString_h

#include <string>
#include <string_view>
#include <map>

namespace Ice {

struct Language_t {
    enum class ID {
        ENGLISH,
        GERMAN
    };

    ID m_lang{ ID::ENGLISH };
    std::string_view m_strID{ "EN" };
};

static constexpr Language_t EnglishLanguage = Language_t{};
static constexpr Language_t GermanLanguage = Language_t{ Language_t::ID::GERMAN, "DE" };

class LocalizedString {
    static std::map<Language_t, std::map<std::string, std::string>> Strings;

    std::string m_strKey;

public:
    LocalizedString(std::string_view strKey) : m_strKey{ strKey } {}
};

}

#endif