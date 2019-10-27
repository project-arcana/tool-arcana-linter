#pragma once

#include <filesystem>
#include <string>

#include <clean-core/vector.hh>

#include <cppast/cpp_file.hpp>

#include "AstParser.hh"

namespace arclint
{
class CodeFile
{
public:
    CodeFile(std::filesystem::path const& path, AstParser const& parser);

    /// Whether this file failed to parse
    [[nodiscard]] bool hasParseErrors() const { return mFileData == nullptr; }

    /// Returns the amount of blacklisted STL headers included by this file
    [[nodiscard]] int getNumBlacklistedSTLIncludes() const;

    [[nodiscard]] int getNumIncludes() const { return int(mIncludedHeaders.size()); }

    bool isHeader = false;             ///< Whether this is a header or not
    bool isExtensionCanonical = false; ///< Whether the file extension is .cc/.hh
    bool hasPragmaOnce = false;        ///< Whether the first line is #pragma once

private:
    std::unique_ptr<cppast::cpp_file> mFileData;
    cc::vector<std::string> mIncludedHeaders;
};
}
