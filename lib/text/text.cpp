#include <locale>
#include <codecvt>

#include "text.hpp"

using namespace text;

#include <iostream>
std::wstring text::to_wide_string(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring result = converter.from_bytes(str);

    return result;
}

Replacer::Replacer(std::wregex&& pattern, std::wstring&& replacement) :
    pattern(std::move(pattern)),
    replacement(std::move(replacement))
{
}

std::wstring Replacer::replace(const std::wstring& str) const {
    return std::regex_replace(str, this->pattern, this->replacement);
}

Cleaner::Cleaner() {}
Cleaner::Cleaner(std::vector<Replacer>&& replacers) : replacers(std::move(replacers)) {
}

Cleaner& Cleaner::emplace_back(std::wregex&& pattern, std::wstring&& replacement) {
    this->replacers.emplace_back(std::move(pattern), std::move(replacement));
    return *this;
}

std::optional<std::wstring> Cleaner::clean(std::wstring str) const {
    const auto original_len = str.length();

    for (const auto& replacer : this->replacers) {
        str = replacer.replace(str);
    }

    if (original_len != str.length()) {
        return str;
    }
    else {
        return std::nullopt;
    }
}

