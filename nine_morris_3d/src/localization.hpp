#pragma once

#include <array>
#include <utility>
#include <initializer_list>

#include <nine_morris_3d_engine/external/resmanager.h++>

class Translation {
public:
    template<typename T>
    constexpr Translation(const char* name, const T& array)
        : m_name(name), m_array(array.data()), m_size(array.size()) {}

    using Element = std::pair<resmanager::HashedStr64, const char*>;

    const char* get_name() const { return m_name; }
    const Element* get_array() const { return m_array; }
    const std::size_t get_size() const { return m_size; }
private:
    const char* m_name {};
    const Element* m_array {};
    std::size_t m_size {};
};

void load_translations(std::initializer_list<Translation> translations);
void set_language(std::size_t index);
const char* get_string(resmanager::HashedStr64 identifier);

inline const char* operator""_L(const char* identifier, std::size_t) {
    return get_string(resmanager::HashedStr64(identifier));
}

#define SM_BEGIN_TRANSLATION(name) inline constexpr std::array translation_##name {
#define SM_TRANSLATE(identifier, text) std::make_pair(identifier, text)
#define SM_TRANSLATE_NEXT() ,
#define SM_END_TRANSLATION() };
