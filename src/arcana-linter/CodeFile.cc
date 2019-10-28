#include "CodeFile.hh"

#include <fstream>
#include <iostream>

#include <cppast/cpp_entity_kind.hpp>

#include <clean-core/assert.hh>

#include "file_util.hh"

namespace
{
std::vector<std::string> whitelisted_stl_headers = {"utility", "type_traits", "atomic", "cstddef", "cstring", "cstdlib", "cstdint"};
}

arclint::CodeFile::CodeFile(const fs::path& path) : mPath(path) { CC_ASSERT(has_cpp_file_extension(path)); }


void arclint::CodeFile::initialize(const arclint::AstParser& parser)
{
    CC_ASSERT(mInitialized == false);

    isHeader = has_cpp_header_extension(mPath);
    isExtensionCanonical = has_canonical_cpp_file_extension(mPath);

    try
    {
        mFileData = parser.parseFile(mPath.string());
    }
    catch (std::runtime_error& e)
    {
        std::cerr << "Error parsing " << mPath.c_str() << '\n';
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

        std::ifstream in_file_raw(mPath.string());
        if (in_file_raw.good())
        {
            std::string first_line;
            std::getline(in_file_raw, first_line);
            hasPragmaOnce = first_line.rfind("#pragma once", 0) == 0;

            if (!hasPragmaOnce)
            {
                std::cout << "This line is not pragma once: " << first_line << std::endl;
            }
        }
        else
        {
            throw std::runtime_error("Failed to re-open file");
        }
    }

    mInitialized = true;
}

std::vector<std::string> arclint::CodeFile::getBlacklistedSTLIncludes() const
{
    std::vector<std::string> res;
    res.reserve(4);

    for (auto const& include : mIncludedHeaders)
    {
        if (include.find('.') == std::string::npos)
        {
            // include has no '.', thus likely an STL header
            if (std::find(whitelisted_stl_headers.begin(), whitelisted_stl_headers.end(), include) == whitelisted_stl_headers.end())
                // header not on the whitelist
                res.push_back(include);
        }
    }

    return res;
}
