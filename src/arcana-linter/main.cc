#include <fstream>
#include <iostream> // TODO rlog
#include <string>   // TODO cc::string

#include <cppast/cpp_file.hpp>
#include <cppast/libclang_parser.hpp>

#include "file_util.hh"

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "usage: ./arcana-linter <file|folder>" << std::endl;
    }
    else
    {
        cppast::cpp_entity_index idx;
        cppast::stderr_diagnostic_logger logger;
        cppast::libclang_compile_config config;
        cppast::libclang_parser parser(type_safe::ref(logger));

        auto const iterate_res = arclint::iterate_regular_files(argv[1], [&](fs::path const& file) {
            if (arclint::has_cpp_file_extension(file))
            {
                std::cout << "parsing " << file.c_str() << std::endl;
                if (!arclint::has_canonical_cpp_file_extension(file))
                {
                    std::cout << "warning: " << file.c_str() << " does not have canonical file extensions .cc/.hh" << std::endl;
                }

                try
                {
                    auto const parse_result = parser.parse(idx, file.string(), config);
                }
                catch (std::exception e)
                {
                    std::cerr << "Error parsing " << file.c_str() << ", parse exception: " << e.what() << std::endl;
                }

                if (parser.error())
                {
                    std::cerr << "Error parsing " << file.c_str() << std::endl;
                }
            }
        });

        if (!iterate_res)
            std::cerr << "Failed to open " << argv[1] << std::endl;
    }
}
