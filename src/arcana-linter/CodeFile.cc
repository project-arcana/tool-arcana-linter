#include "CodeFile.hh"

#include <fstream>
#include <iostream>

#include <cppast/cpp_entity_kind.hpp>

#include <clean-core/assert.hh>

#include "file_util.hh"

namespace
{
std::vector<std::string> blacklisted_stl_headers = {"csignal",
                                                    "csetjmp",
                                                    "cstdarg",
                                                    "typeinfo",
                                                    "typeindex",
                                                    "bitset",
                                                    "functional",
                                                    "ctime",
                                                    "chrono",
                                                    "initializer_list",
                                                    "tuple",
                                                    "any",
                                                    "optional",
                                                    "variant",
                                                    "compare",
                                                    "version",
                                                    "source_location",
                                                    "new",
                                                    "memory",
                                                    "scoped_allocator",
                                                    "memory_resource",
                                                    "limits",
                                                    "exception",
                                                    "stdexception",
                                                    "cassert",
                                                    "system_error",
                                                    "cerrno",
                                                    "cctype",
                                                    "cwctype",
                                                    "cwchar",
                                                    "cuchar",
                                                    "string_view",
                                                    "charconv",
                                                    "format",
                                                    "array",
                                                    "vector",
                                                    "deque",
                                                    "list",
                                                    "forward_list",
                                                    "set",
                                                    "map",
                                                    "unordered_set",
                                                    "unordered_map",
                                                    "stack",
                                                    "queue",
                                                    "span",
                                                    "iterator",
                                                    "ranges",
                                                    "algorithm",
                                                    "execution",
                                                    "complex",
                                                    "valarray",
                                                    "random",
                                                    "numeric",
                                                    "ratio",
                                                    "cfenv",
                                                    "bit",
                                                    "iosfwd",
                                                    "ios",
                                                    "istream",
                                                    "ostream",
                                                    "iostream",
                                                    "streambuf",
                                                    "locale",
                                                    "clocale",
                                                    "codecvt",
                                                    "regex",
                                                    "thread",
                                                    "mutex",
                                                    "stop_token",
                                                    "shared_mutex",
                                                    "future",
                                                    "semaphore",
                                                    "latch",
                                                    "barrier",
                                                    "filesystem"};

}

arclint::CodeFile::CodeFile(const fs::path& path, const arclint::AstParser& parser)
  : isHeader(has_cpp_header_extension(path)), isExtensionCanonical(has_canonical_cpp_file_extension(path))
{
    CC_ASSERT(has_cpp_file_extension(path));

    try
    {
        mFileData = parser.parseFile(path.string());
    }
    catch (std::runtime_error& e)
    {
        std::cerr << "Error parsing " << path.c_str() << '\n';
        std::cerr << "  " << e.what() << '\n';
    }

    if (mFileData)
    {
        for (auto const& e : *mFileData)
        {
            //                        std::cout << "Kind: " << to_string(e.kind()) << ", name: " << e.name() << std::endl;
            if (e.kind() == cppast::cpp_entity_kind::include_directive_t)
            {
                mIncludedHeaders.push_back(e.name());
            }
        }

        std::ifstream in_file_raw(path.string());
        if (in_file_raw.good())
        {
            std::string first_line;
            std::getline(in_file_raw, first_line);
            hasPragmaOnce = first_line.rfind("#pragma once", 0) == 0;
        }
        else {
            throw std::runtime_error("Failed to re-open file");
        }
    }
}

int arclint::CodeFile::getNumBlacklistedSTLIncludes() const
{
    auto res = 0;

    for (auto const& include : mIncludedHeaders)
    {
        // TODO: Maybe this should use a whitelist instead, and check if the include has no '.' inside of it
        if (std::find(blacklisted_stl_headers.begin(), blacklisted_stl_headers.end(), include) != blacklisted_stl_headers.end())
        {
            ++res;
        }
    }

    return res;
}
