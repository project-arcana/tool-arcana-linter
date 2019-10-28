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
    warning_collection warnings;

    [[nodiscard]] bool has_any_warnings() const
    {
        return warnings.fatal_parse_error || warnings.noncanonical_file_extensions || warnings.missing_pragma_once || !blacklisted_includes.empty();
    }
};

class WarningLog
{
public:
    void registerFile(CodeFile const& file)
    {
        file_info file_info;
        file_info.file_name = file.getFileName();

        CC_DEFER
        {
            if (file_info.has_any_warnings())
            {
                mFileInfos.push_back(file_info);
            }
        };

        if (!file.isExtensionCanonical)
        {
            file_info.warnings.noncanonical_file_extensions = true;
            ++mNumNoncanonicalFileExtensions;
        }

        if (file.hasParseErrors())
        {
            file_info.warnings.fatal_parse_error = true;
            ++mNumParseErrors;
            return;
        }

        if (file.isHeader)
        {
            ++mNumHeaders;

            if (!file.hasPragmaOnce)
            {
                file_info.warnings.missing_pragma_once = true;
                ++mNumMissingPragmaOnce;
            }

            file_info.blacklisted_includes = file.getBlacklistedSTLIncludes();
            mNumBlacklistedSTLIncludes += file_info.blacklisted_includes.size();
        }
        else
        {
            ++mNumTranslationUnits;
        }
    }

    void printSummary() const
    {
        std::cout << "parse completed (" << mNumTranslationUnits << " TUs, " << mNumHeaders << " headers)" << std::endl;

        // TODO
        if (mNumBlacklistedSTLIncludes > 0)
        {
            std::cout << "amount of blacklisted STL header includes: " << mNumBlacklistedSTLIncludes << std::endl;

            for (auto const& file : mFileInfos)
            {
                if (!file.blacklisted_includes.empty())
                {
                    std::cout << "  in " << file.file_name << ": ";
                    auto first = true;
                    for (auto const& header : file.blacklisted_includes)
                    {
                        if (!first)
                            std::cout << ", ";
                        std::cout << "<" << header << ">";
                        if (first)
                            first = false;
                    }
                    std::cout << std::endl;
                }
            }
        }

        if (mNumNoncanonicalFileExtensions > 0)
        {
            std::cout << "amount of files without canonical cc/hh extension: " << mNumNoncanonicalFileExtensions << std::endl;
            for (auto const& file : mFileInfos)
            {
                if (file.warnings.noncanonical_file_extensions)
                {
                    std::cout << "  in " << file.file_name << std::endl;
                }
            }
        }
        if (mNumParseErrors > 0)
        {
            std::cout << "amount of files failing to parse: " << mNumParseErrors << std::endl;
            for (auto const& file : mFileInfos)
            {
                if (file.warnings.fatal_parse_error)
                {
                    std::cout << "  in " << file.file_name << std::endl;
                }
            }
        }

        if (mNumMissingPragmaOnce > 0)
        {
            std::cout << "amount of headers missing #pragma once: " << mNumMissingPragmaOnce << std::endl;
            for (auto const& file : mFileInfos)
            {
                if (file.warnings.missing_pragma_once)
                {
                    std::cout << "  in " << file.file_name << std::endl;
                }
            }
        }
    }

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

    /// The amount of parse errors
    int mNumParseErrors = 0;
};

}
