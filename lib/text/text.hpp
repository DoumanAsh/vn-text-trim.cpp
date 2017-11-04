#pragma once

#include <string>
#include <optional>
#include <vector>
#include <regex>

namespace text {
    class Replacer {
        private:
            std::wregex pattern;
            std::wstring replacement;
        public:
            Replacer(std::wregex&& pattern, std::wstring&& replacement);
            ///Replaces text according to pattern and provided replacement text.
            std::wstring replace(const std::wstring&) const;
    };

    /**
     * Text cleaner using bunch of regexes.
     */
    class Cleaner {
        private:
            std::vector<Replacer> replacers;

        public:
            Cleaner();
            explicit Cleaner(std::vector<Replacer>&& replacers);
            Cleaner& emplace_back(std::wregex&& pattern, std::wstring&& replacement);
            ///Cleans text.
            std::optional<std::wstring> clean(std::wstring) const;
    };

    std::wstring to_wide_string(const std::string& str);
}
