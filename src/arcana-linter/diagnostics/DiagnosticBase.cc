#include "DiagnosticBase.hh"

#include <iostream>

void arclint::diag::DiagnosticBase::printSummary() const
{
    if (mNumOffenses > 0)
    {
        std::cout << "Diagnostic: " << getName() << ", " << mNumOffenses << " warnings" << std::endl;
        for (auto const& file : mOffendingFiles)
        {
            std::cout << "  in " << file << std::endl;
        }
    }
}

void arclint::diag::DiagnosticBase::addOffendingFile(std::string_view path, std::string_view reason, int offense_count)
{
    mNumOffenses += offense_count;
    if (!reason.empty())
        mOffendingFiles.push_back(std::string(path) + ": " + std::string(reason));
    else
        mOffendingFiles.emplace_back(path);
}
