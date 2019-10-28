#pragma once

#include <cppast/libclang_parser.hpp>

#include "common/cxxopts.hh"

namespace arclint
{
class AstParser
{
public:
    AstParser(cxxopts::ParseResult const& options);

    [[nodiscard]] std::unique_ptr<cppast::cpp_file> parseFile(std::string const& filename) const;

private:
    cppast::libclang_compile_config mConfig;
    cppast::stderr_diagnostic_logger mLogger;
};

}
