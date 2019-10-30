#include "WarningLog.hh"

void arclint::WarningLog::registerFile(const arclint::CodeFile& file)
{
    if (file.hasParseErrors())
    {
        ++mNumParseErrors;
        return;
    }

    if (file.isHeader)
        ++mNumHeaders;
    else
        ++mNumTranslationUnits;

    for (auto const& diag : mDiagnostics)
        diag->registerFile(file);
}

void arclint::WarningLog::printSummary(double time_spent_ms) const
{
    std::cout << "parsed " << mNumTranslationUnits << " TUs and " << mNumHeaders << " headers in " << time_spent_ms << " ms" << std::endl;

    for (auto const& diag : mDiagnostics)
        diag->printSummary();
}
