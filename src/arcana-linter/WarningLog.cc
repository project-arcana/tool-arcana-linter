#include "WarningLog.hh"

void arclint::WarningLog::registerFile(const arclint::CodeFile& file)
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

    file_info.funcs_missing_noexcept = file.getFunctionsMissingNoexcept();
    mNumMissingNoexcept += int(file_info.funcs_missing_noexcept.size());
}

void arclint::WarningLog::printSummary(double time_spent_ms) const
{
    std::cout << "parsed " << mNumTranslationUnits << " TUs and " << mNumHeaders << " headers in " << time_spent_ms << " ms" << std::endl;

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

    if (mNumMissingNoexcept > 0)
    {
        std::cout << "amount of functions missing noexcept specifier: " << mNumMissingNoexcept << std::endl;

        for (auto const& file : mFileInfos)
        {
            if (!file.funcs_missing_noexcept.empty())
            {
                std::cout << "  in " << file.file_name << ": ";
                auto first = true;
                for (auto const& func : file.funcs_missing_noexcept)
                {
                    if (!first)
                        std::cout << ", ";
                    std::cout << func;
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
