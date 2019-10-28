#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <cppast/cpp_file.hpp>

#include "AstParser.hh"

namespace arclint
{
class CodeFile
{
public:
    CodeFile(std::filesystem::path const& path);

    void initialize(AstParser const& parser);

    /// Whether this file failed to parse
    [[nodiscard]] bool hasParseErrors() const { return mFileData == nullptr; }

    /// Returns the blacklisted STL headers included by this file
    [[nodiscard]] std::vector<std::string> getBlacklistedSTLIncludes() const;

    [[nodiscard]] int getNumIncludes() const { return int(mIncludedHeaders.size()); }

    [[nodiscard]] std::string getFileName() const { return mPath.string(); }

    [[nodiscard]] bool isInitialized() const { return mInitialized; }

    bool isHeader = false;             ///< Whether this is a header or not
    bool isExtensionCanonical = false; ///< Whether the file extension is .cc/.hh
    bool hasPragmaOnce = false;        ///< Whether the first line is #pragma once

private:
    std::filesystem::path mPath;
    bool mInitialized = false;
    std::unique_ptr<cppast::cpp_file> mFileData;
    std::vector<std::string> mIncludedHeaders;
};
}
