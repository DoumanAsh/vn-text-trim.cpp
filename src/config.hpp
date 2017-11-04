#pragma once

#include <variant>
#include <string>

#include <text/text.hpp>

namespace config {
    struct Config {
        std::vector<text::Replacer> replace;
    };

    /**
     * Opens config file
     *
     * @returns Config on success or Error description.
     */
    std::variant<Config, std::string> open(const char* file);
}
