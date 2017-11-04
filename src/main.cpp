#include <iostream>
#include <clocale>

#include <clipboard/clipboard.hpp>

#include "cli.hpp"
#include "config.hpp"

static inline text::Cleaner init_cleaner(config::Config&& config) {
    return text::Cleaner(std::move(config.replace));
}

static inline config::Config open_config(const char* file) {
    auto config_file = config::open(file);

    if (auto error = std::get_if<std::string>(&config_file)) {
        std::cerr << *error << "\n";
        exit(1);
    }

    return *std::get_if<config::Config>(&config_file);
}

int main(int argc, char* argv[]) {
    std::setlocale(LC_ALL, "en_US.UTF-8");

    cli::Parser parser;
    parser.author("Douman <douman@gmx.se>")
          .about("Simple ITH text cleaner.")
          .version(PR_VERSION);

    auto args = parser.parse(argc, argv);

    const auto cleaner = init_cleaner(std::move(open_config(args.config.c_str())));

    const auto cb = [&cleaner]() {
        const Clipboard clip;
        const auto text = clip.get_wstring();
        if (text.size() > 0) {
            const auto result = cleaner.clean(text);
            if (result.has_value()) {
                while (!clip.set_string(*result)) {
                    std::cerr << "Failed to set new clipboard! Try again...\n";
                }
            }
        }
    };

    std::cout << "Start...\n";
    ClipboardMaster(cb).run();

    return 0;
}
