#pragma once

#include <clean-core/defer.hh>
//#include <clean-core/vector.hh>

#include <vector>

#include "CodeFile.hh"

namespace arclint
{
struct warning_collection
{
    bool missing_pragma_once = false;
    bool noncanonical_file_extensions = false;
    bool fatal_parse_error = false;
};

struct file_info
{
    std::string file_name;
    std::vector<std::string> blacklisted_includes;
    std::vector<std::string> funcs_missing_noexcept;
    warning_collection warnings;

    [[nodiscard]] bool has_any_warnings() const
    {
        return warnings.fatal_parse_error || warnings.noncanonical_file_extensions || warnings.missing_pragma_once || !blacklisted_includes.empty() || !funcs_missing_noexcept.empty();
    }
};

class WarningLog
{
public:
    void registerFile(CodeFile const& file);

    void printSummary() const;

private:
    int mNumTranslationUnits = 0;
    int mNumHeaders = 0;

    std::vector<file_info> mFileInfos;

    /// The amount of blacklisted STL headers included (only in header files)
    int mNumBlacklistedSTLIncludes = 0;

    /// The amount of files not adhering to the canonical cc/hh naming
    int mNumNoncanonicalFileExtensions = 0;

    /// The amount of headers without #pragma once
    int mNumMissingPragmaOnce = 0;

    int mNumParseErrors = 0;

    int mNumMissingNoexcept = 0;
};

}
