#include <fstream>

#pragma warning(push)
#pragma warning(disable: 4996)
#include <toml/toml.h>
#pragma warning(pop)

#include <text/text.hpp>
#include "config.hpp"

#include <windows.h>

using namespace config;

std::variant<Config, std::string> config::open(const char* file) {
    std::ifstream file_stream(file);

    if (file_stream.fail()) {
        return std::string("Cannot open config file: ") + file;
    }

    const auto pr = toml::parse(file_stream);

    if (!pr.valid()) {
        return pr.errorReason;
    }

    Config result;

    if (const auto replace = pr.value.find("replace")) {
        if (replace->is<toml::Array>()) {
            for (const toml::Value& value : replace->as<toml::Array>()) {
                if (value.is<toml::Table>()) {
                    const auto table = value.as<toml::Table>();

                    const auto pattern = table.find("pattern");
                    if (pattern == table.end()) return std::string("Missing pattern key!");
                    const auto replacement = table.find("replacement");
                    if (replacement == table.end()) return std::string("Missing replacement key!");

                    if (!pattern->second.is<std::string>()) return std::string("pattern key is not a string!");
                    auto pattern_value = text::to_wide_string(pattern->second.as<std::string>());
                    if (!replacement->second.is<std::string>()) return std::string("replacement key is not a string!");
                    auto replacement_value = text::to_wide_string(replacement->second.as<std::string>());

                    result.replace.emplace_back(std::wregex(pattern_value), std::move(replacement_value));
                }
                else {
                    return std::string("Unexpected replace pattern!");
                }
            }
        }
    }

    return result;
}
