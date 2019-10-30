#pragma once

#include <cppast/libclang_parser.hpp>

#include "common/cxxopts.hh"

namespace arclint
{
class silent_diagnostic_logger final : public cppast::diagnostic_logger
{
public:
    using diagnostic_logger::diagnostic_logger;

private:
    bool do_log(const char*, const cppast::diagnostic&) const override { return false; }
};

class AstParser
{
public:
    AstParser(cxxopts::ParseResult const& options);

    [[nodiscard]] std::unique_ptr<cppast::cpp_file> parseFile(std::string const& filename) const;

private:
    cppast::libclang_compile_config mConfig;
    silent_diagnostic_logger mLogger;
};

}
