#pragma once

#include "CodeFile.hh"

namespace arclint
{
class WarningLog
{
public:
    void registerFile(CodeFile const& file)
    {
        if (!file.isExtensionCanonical)
            ++mNumNoncanonicalFileExtensions;

        if (file.hasParseErrors())
        {
            ++mNumParseErrors;
            return;
        }

        if (file.isHeader)
        {
            ++mNumHeaders;

            if (!file.hasPragmaOnce)
                ++mNumMissingPragmaOnce;

            mNumBlacklistedSTLIncludes += file.getNumBlacklistedSTLIncludes();
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
            std::cout << "amount of blacklisted STL header includes: " << mNumBlacklistedSTLIncludes << std::endl;

        if (mNumNoncanonicalFileExtensions > 0)
            std::cout << "amount of files without canonical cc/hh extension: " << mNumNoncanonicalFileExtensions << std::endl;

        if (mNumParseErrors > 0)
            std::cout << "amount of files failing to parse: " << mNumParseErrors << std::endl;

        if (mNumMissingPragmaOnce > 0)
            std::cout << "amount of headers missing #pragma once: " << mNumMissingPragmaOnce << std::endl;
    }

private:
    int mNumTranslationUnits = 0;
    int mNumHeaders = 0;

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
