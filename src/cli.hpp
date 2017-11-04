#include <string>
#include <sstream>
#include <filesystem>
namespace fs = std::experimental::filesystem;

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>

namespace cli {
    namespace po = boost::program_options;

    struct Args {
    public:
        std::string config;
    };

    class Parser {
    private:
        //d_* members belong to description
        const char* d_about = nullptr;
        const char* d_author = nullptr;
        const char* d_version = nullptr;

        std::string description() const {
            std::ostringstream result;
            if (d_version) {
                result << d_version << "\n";
            }
            if (d_author) {
                result << d_author << "\n";
            }
            if (d_about) {
                result << "\n" << d_about << "\n\n";
            }

            result << "USAGE:\n"
                << "    vn-text-trim [OPTIONS]\n\n"
                << "OPTIONS";

            return result.str();
        }

    public:
        /**
         * Initializes Parser.
         */
        Parser() noexcept {};

        ///Adds about to help message.
        Parser& about(const char* about) noexcept {
            d_about = about;
            return *this;
        }
        ///Adds author to help message.
        Parser& author(const char* author) noexcept {
            d_author = author;
            return *this;
        }
        ///Adds author to help message.
        Parser& version(const char* version) noexcept {
            d_version = version;
            return *this;
        }

        Args parse(int argc, char* argv[]) {
            fs::path current_exe(argv[0]);
            current_exe.replace_extension("toml");

            Args result;
            result.config = current_exe.string();

            auto description = this->description();

            po::options_description desc(description.c_str());

            desc.add_options()("config,c", po::value<std::string>(&result.config)->multitoken(), "Specifies configuration file to use.");
            desc.add_options()("help,h", "Prints help information.");
            if (d_version) {
                desc.add_options()("version", "Prints version information.");
            }

            po::variables_map vm;
            try {
                const auto styles = po::command_line_style::allow_sticky |
                                    po::command_line_style::allow_short |
                                    po::command_line_style::unix_style;
                po::store(po::command_line_parser(argc, argv).options(desc)
                                                             .style(styles)
                                                             .run(),
                          vm);
                po::notify(vm);
            }
            catch (po::error& err) {
                std::cerr << err.what() << "\n";
                exit(1);
            }

            if (vm.count("help")) {
                std::cout << desc << "\n";
                exit(0);
            }
            else if (vm.count("version")) {
                std::cout << d_version;
                exit(0);
            }

            return result;
        }
    };
}
